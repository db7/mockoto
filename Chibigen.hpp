#ifndef MOCKOTO_CHIBIGEN_HPP
#define MOCKOTO_CHIBIGEN_HPP

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>

#include "Config.hpp"

using namespace clang;

namespace mockoto {

class ChibigenVisitor : public RecursiveASTVisitor<ChibigenVisitor> {
  ASTContext *Context;
  mockoto::Config Config;
  llvm::StringRef InFile;
  std::set<std::string> emitted;

public:
  explicit ChibigenVisitor(ASTContext *Context, mockoto::Config Config,
                           llvm::StringRef InFile)
      : Context(Context), Config(Config), InFile(InFile) {
    llvm::outs() << ";; WARNING: auto-generated file - Changes will be lost!\n";
    llvm::outs() << ";; This file is intended for chibi-ffi.\n\n";
    for (auto inFile : Config.includeFiles) {
      llvm::outs() << "(c-include \"" << inFile << "\")\n";
    }
    llvm::outs() << "\n";
  }

  bool VisitNamedDecl(NamedDecl *NamedDecl) {
    if (!shouldProcess(NamedDecl))
      return true;

    const FunctionDecl *fdecl = dyn_cast<FunctionDecl>(NamedDecl);
    if (fdecl != NULL) {
      emitFunction(fdecl);
      return true;
    }

    const TypedefNameDecl *tdecl = dyn_cast<TypedefNameDecl>(NamedDecl);
    if (tdecl != NULL) {
      emitTypedef(tdecl);
      return true;
    }

    const EnumDecl *edecl = dyn_cast<EnumDecl>(NamedDecl);
    if (edecl != NULL) {
      emitEnum(edecl);
      return true;
    }

    const RecordDecl *rdecl = dyn_cast<RecordDecl>(NamedDecl);
    if (rdecl != NULL) {
      emitRecord(rdecl);
      return true;
    }

    return true;
  }

private:
  static bool endsWith(const std::string &s, const std::string &suffix) {
    if (suffix.size() > s.size())
      return false;
    return std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
  }

  bool shouldProcess(const NamedDecl *decl) const {
    SourceManager &sm = Context->getSourceManager();
    SourceLocation loc = sm.getExpansionLoc(decl->getLocation());
    if (!loc.isValid())
      return false;

    std::string path = sm.getFilename(loc).str();
    if (path.empty())
      return false;

    for (auto pfx : Config.excludePatterns) {
      if (path.find(pfx) != std::string::npos)
        return false;
    }

    if (Config.includeFiles.empty())
      return true;

    for (auto h : Config.includeFiles) {
      if (path == h)
        return true;
      if (endsWith(path, "/" + h))
        return true;
      if (endsWith(path, h))
        return true;
      size_t slash = h.find_last_of("/\\");
      if (slash != std::string::npos) {
        std::string base = h.substr(slash + 1);
        if (!base.empty() && endsWith(path, "/" + base))
          return true;
      }
    }
    return false;
  }

  static std::string toSymbol(const std::string &raw) {
    if (raw.empty())
      return "anon";
    std::string out;
    out.reserve(raw.size());
    for (unsigned char c : raw) {
      if (std::isalnum(c) || c == '_')
        out.push_back((char)c);
      else
        out.push_back('_');
    }
    if (!out.empty() && std::isdigit((unsigned char)out[0]))
      out = "t_" + out;
    return out;
  }

  static std::string mapBuiltin(const BuiltinType *bt) {
    switch (bt->getKind()) {
    case BuiltinType::Void:
      return "void";
    case BuiltinType::Bool:
      return "bool";
    case BuiltinType::Char_S:
    case BuiltinType::SChar:
      return "char";
    case BuiltinType::Char_U:
    case BuiltinType::UChar:
      return "unsigned-char";
    case BuiltinType::Short:
      return "short";
    case BuiltinType::UShort:
      return "unsigned-short";
    case BuiltinType::Int:
      return "int";
    case BuiltinType::UInt:
      return "unsigned-int";
    case BuiltinType::Long:
      return "long";
    case BuiltinType::ULong:
      return "unsigned-long";
    case BuiltinType::LongLong:
      return "int64_t";
    case BuiltinType::ULongLong:
      return "uint64_t";
    case BuiltinType::Float:
      return "float";
    case BuiltinType::Double:
      return "double";
    default:
      return "int";
    }
  }

  std::string toTypeExpr(QualType type) const {
    type.removeLocalFastQualifiers();

    if (const ElaboratedType *et =
            dyn_cast<ElaboratedType>(type.getTypePtr())) {
      return toTypeExpr(et->getNamedType());
    }

    if (type->isPointerType()) {
      QualType p = type->getPointeeType();
      p.removeLocalFastQualifiers();
      if (p->isVoidType())
        return "(pointer void)";
      if (p->isFunctionType())
        return "(pointer void)";
      if (p->isCharType())
        return "string";
      if (p->isStructureType() || p->isUnionType() || p->isEnumeralType())
        return toTypeExpr(p);
      if (const TypedefType *tt = dyn_cast<TypedefType>(p.getTypePtr())) {
        QualType ut = tt->getDecl()->getUnderlyingType();
        ut.removeLocalFastQualifiers();
        if (ut->isStructureType() || ut->isUnionType() || ut->isEnumeralType())
          return toTypeExpr(p);
      }
      return "(pointer " + toTypeExpr(p) + ")";
    }

    if (type->isReferenceType()) {
      QualType r = type->getPointeeType();
      return "(pointer " + toTypeExpr(r) + ")";
    }

    if (const ConstantArrayType *at = Context->getAsConstantArrayType(type)) {
      std::string elem = toTypeExpr(at->getElementType());
      return "(array " + elem + " " +
             std::to_string(at->getSize().getLimitedValue()) + ")";
    }

    if (const BuiltinType *bt = dyn_cast<BuiltinType>(type.getTypePtr())) {
      return mapBuiltin(bt);
    }

    if (const TypedefType *tt = dyn_cast<TypedefType>(type.getTypePtr())) {
      return toSymbol(tt->getDecl()->getQualifiedNameAsString());
    }

    if (type->isEnumeralType()) {
      if (const TagDecl *td = type->getAsTagDecl()) {
        return toSymbol(td->getQualifiedNameAsString());
      }
      return "int";
    }

    if (type->isStructureType() || type->isUnionType()) {
      if (const RecordType *rt = type->getAsStructureType()) {
        return toSymbol(rt->getDecl()->getQualifiedNameAsString());
      }
      if (const RecordType *rt = type->getAsUnionType()) {
        return toSymbol(rt->getDecl()->getQualifiedNameAsString());
      }
      return "(pointer void)";
    }

    std::string ts = type.getAsString();
    static const std::map<std::string, std::string> aliases = {
        {"uint8_t", "uint8_t"},   {"uint16_t", "uint16_t"},
        {"uint32_t", "uint32_t"}, {"uint64_t", "uint64_t"},
        {"int8_t", "int8_t"},     {"int16_t", "int16_t"},
        {"int32_t", "int32_t"},   {"int64_t", "int64_t"},
        {"size_t", "size_t"},     {"ssize_t", "ssize_t"},
        {"bool", "bool"},
    };
    auto it = aliases.find(ts);
    if (it != aliases.end())
      return it->second;
    return toSymbol(ts);
  }

  void emitFunction(const FunctionDecl *decl) {
    if (!decl->hasPrototype())
      return;
    if (decl->getStorageClass() == SC_Static)
      return;

    std::string name = toSymbol(decl->getQualifiedNameAsString());
    std::string key = "func:" + name;
    if (emitted.find(key) != emitted.end())
      return;
    emitted.insert(key);

    auto ftype = decl->getType()->getAs<FunctionProtoType>();
    if (ftype == NULL)
      return;

    llvm::outs() << ";; function " << name << "\n";
    llvm::outs() << "(define-c " << toTypeExpr(ftype->getReturnType()) << " "
                 << name << " (";
    if (ftype->getNumParams() == 0) {
      llvm::outs() << "void";
    } else {
      for (unsigned i = 0; i < ftype->getNumParams(); i++) {
        if (i)
          llvm::outs() << " ";
        llvm::outs() << toTypeExpr(ftype->getParamType(i));
      }
    }
    llvm::outs() << "))\n\n";
  }

  void emitTypedef(const TypedefNameDecl *decl) {
    std::string name = toSymbol(decl->getQualifiedNameAsString());
    std::string key = "typedef:" + name;
    if (emitted.find(key) != emitted.end())
      return;
    emitted.insert(key);

    llvm::outs() << ";; typedef " << name << "\n";
    llvm::outs() << "(c-typedef " << toTypeExpr(decl->getUnderlyingType())
                 << " " << name << ")\n\n";
    if (isIntegerLike(decl->getUnderlyingType())) {
      llvm::outs() << "(define-c-int-type " << name << ")\n\n";
    }
  }

  void emitEnum(const EnumDecl *decl) {
    if (!decl->isCompleteDefinition())
      return;
    std::string name = toSymbol(decl->getQualifiedNameAsString());
    if (name.empty() || name == "anon")
      return;
    std::string key = "enum:" + name;
    if (emitted.find(key) != emitted.end())
      return;
    emitted.insert(key);

    llvm::outs() << ";; enum " << name << "\n";
    llvm::outs() << "(define-c-enum " << name;
    for (auto it = decl->enumerator_begin(); it != decl->enumerator_end();
         it++) {
      llvm::outs() << " " << toSymbol(it->getNameAsString());
    }
    llvm::outs() << ")\n\n";
  }

  void emitRecord(const RecordDecl *decl) {
    if (!decl->isCompleteDefinition())
      return;
    if (decl->isAnonymousStructOrUnion())
      return;

    std::string name = toSymbol(decl->getQualifiedNameAsString());
    if (name.empty() || name == "anon")
      return;

    std::string kind = decl->isUnion() ? "union:" : "struct:";
    std::string key = kind + name;
    if (emitted.find(key) != emitted.end())
      return;
    emitted.insert(key);

    llvm::outs() << ";; " << (decl->isUnion() ? "union " : "struct ") << name
                 << "\n";
    llvm::outs() << "("
                 << (decl->isUnion() ? "define-c-union " : "define-c-struct ")
                 << name;

    unsigned idx = 0;
    for (auto it = decl->field_begin(); it != decl->field_end(); it++, idx++) {
      std::string fname = it->getNameAsString();
      if (fname.empty())
        fname = "field_" + std::to_string(idx);
      llvm::outs() << "\n  (" << toTypeExpr(it->getType()) << " "
                   << toSymbol(fname) << ")";
    }
    llvm::outs() << ")\n\n";
  }

  bool isIntegerLike(QualType type) const {
    type.removeLocalFastQualifiers();

    if (const ElaboratedType *et =
            dyn_cast<ElaboratedType>(type.getTypePtr())) {
      return isIntegerLike(et->getNamedType());
    }

    if (type->isEnumeralType())
      return true;

    if (const BuiltinType *bt = dyn_cast<BuiltinType>(type.getTypePtr())) {
      switch (bt->getKind()) {
      case BuiltinType::Bool:
      case BuiltinType::Char_U:
      case BuiltinType::UChar:
      case BuiltinType::Char_S:
      case BuiltinType::SChar:
      case BuiltinType::Short:
      case BuiltinType::UShort:
      case BuiltinType::Int:
      case BuiltinType::UInt:
      case BuiltinType::Long:
      case BuiltinType::ULong:
      case BuiltinType::LongLong:
      case BuiltinType::ULongLong:
        return true;
      default:
        return false;
      }
    }

    if (const TypedefType *tt = dyn_cast<TypedefType>(type.getTypePtr())) {
      return isIntegerLike(tt->getDecl()->getUnderlyingType());
    }

    return false;
  }
};

} // namespace mockoto

#endif
