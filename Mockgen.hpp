#ifndef MOCKOTO_MOCKGEN_HPP
#define MOCKOTO_MOCKGEN_HPP

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream> //std::stringstream

#include "ActionFactory.hpp"
#include "Config.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

namespace mockoto {

class MockgenVisitor : public RecursiveASTVisitor<mockoto::MockgenVisitor> {
  ASTContext *Context;
  mockoto::Config Config;
  llvm::StringRef InFile;

public:
  explicit MockgenVisitor(ASTContext *Context, mockoto::Config Config,
                          llvm::StringRef InFile)
      : Context(Context), Config(Config), InFile(InFile) {
    llvm::outs() << "// WARNING: auto-generated file - Changes will be lost!\n";
    if (Config.mode == Config::Mode::MOCK_H) {
      llvm::outs() << "#ifndef MOCKOTO__H\n"
                   << "#define MOCKOTO__H\n\n";
    }

    for (auto inFile : Config.includeFiles)
      llvm::outs() << "#include \"" //
                   << inFile        //
                   << "\"\n";

    if (Config.mode == Config::Mode::MOCK_C)
      llvm::outs() << "\n#define weak_mock __attribute__((weak))\n";
  }
  ~MockgenVisitor() {
    if (Config.mode == Config::Mode::MOCK_H) {
      llvm::outs() << "\n#endif\n";
    }
  }

  bool VisitNamedDecl(NamedDecl *NamedDecl) {
    SourceManager &sm = Context->getSourceManager();
    std::string location = NamedDecl->getLocation().printToString(sm);

    for (auto pfx : Config.excludePatterns)
      if (location.find(pfx) != std::string::npos)
        return true;

    // remove column number from location string
    size_t from = location.find(":", location.find(":") + 1);
    location = location.erase(from, location.length() - from);

    std::string name = NamedDecl->getQualifiedNameAsString();
    std::stringstream definition;
    std::set<std::string> dependencies;

    const FunctionDecl *decl = dyn_cast<FunctionDecl>(NamedDecl);
    if (decl == NULL)
      return true;

    if (!decl->hasPrototype()) {
      llvm::outs() << "// ERROR: function without prototype: " << name
                   << "\n\n";
      return true;
    }

    if (decl->getStorageClass() == SC_Static ||
        decl->getStorageClass() == SC_Extern) {
      // llvm::outs() << "// ignore: static or extern function\n\n";
      return true;
    }

    llvm::outs() << "\n";
    llvm::outs() << "// function: " << name << "\n";
    if (Config.printSourcePath)
      llvm::outs() << "// location: " << location << "\n";

    auto ftype = decl->getType()->getAs<clang::FunctionProtoType>();
    auto returnType = ftype->getReturnType();
    auto returnTypeName = returnType.getAsString();

    // -----------------------------------------------------------------------
    // hook cb
    // -----------------------------------------------------------------------
    std::string hookType = "mockoto_" + name + "_f";
    llvm::outs() << "typedef " << returnTypeName << " (*" << hookType << ") (";
    for (int i = 0; i < ftype->getNumParams(); i++) {
      auto paramType = ftype->getParamTypes()[i];
      llvm::outs() << (i == 0 ? "" : ", ") << paramType.getAsString();
    }
    llvm::outs() << ");\n";

    // hook variable
    std::string hookName = "_" + name + "_hook";
    if (Config.mode == Config::Mode::MOCK_C)
      llvm::outs() << "static " << hookType << " " << hookName << ";\n";

    // hook setter
    llvm::outs() << "void " //
                 << "mockoto_" << name << "_hook(" << hookType << " cb)"
                 << (Config.mode == Config::Mode::MOCK_C
                         ? "\n{\n\t" + hookName + " = cb;\n}"
                         : ";")
                 << "\n";

    // -----------------------------------------------------------------------
    // default return value
    // -----------------------------------------------------------------------
    auto retvalName = "_" + name + "_retval";
    if (returnTypeName.find("void") == std::string::npos ||
        returnType->isPointerType()) {

      if (Config.mode == Config::Mode::MOCK_C)
        llvm::outs() << "static " << returnTypeName << " " //
                     << retvalName << ";\n";

      llvm::outs() << "void " //
                   << "mockoto_" + name + "_returns(" + returnTypeName + " ret)"
                   << (Config.mode == Config::Mode::MOCK_C
                           ? "\n{\n\t" + retvalName + " = ret;\n}"
                           : ";")
                   << "\n";
    }

    // -----------------------------------------------------------------------
    // called count
    // -----------------------------------------------------------------------
    auto countName = "_" + name + "_count";
    if (Config.mode == Config::Mode::MOCK_C)
      llvm::outs() << "static int " << countName << ";\n";

    llvm::outs() << "int " //
                 << "mockoto_" + name + "_called(void)"
                 << (Config.mode == Config::Mode::MOCK_C
                         ? "\n{\n\tint count = " + countName + ";\n\t" +
                               countName + " = 0;\n\treturn count;\n}"
                         : ";")
                 << "\n";

    // we are done if in header mode
    if (Config.mode == Config::Mode::MOCK_H)
      return true;

    // -----------------------------------------------------------------------
    // signature
    // -----------------------------------------------------------------------
    llvm::outs() << "weak_mock " << returnTypeName << "\n";
    llvm::outs() << name << "(";
    std::vector<std::string> paramNames;
    if (ftype->getNumParams() == 0) {
      llvm::outs() << "void)\n";
    } else {
      for (int i = 0; i < ftype->getNumParams(); i++) {
        llvm::outs() << (i == 0 ? "" : ", ");

        std::string paramName = "__param_" + std::to_string(i);
        paramNames.push_back(paramName);
        auto paramType = ftype->getParamTypes()[i];
        if (paramType->isFunctionPointerType()) {
          auto fptype =
              paramType->getPointeeType()->getAs<clang::FunctionType>();
          auto fftype = fptype->getAs<clang::FunctionProtoType>();
          llvm::outs() << fftype->getReturnType().getAsString() << "(*"
                       << paramName << ")(";
          if (fftype->getNumParams() == 0) {
            llvm::outs() << "void)";
          } else {
            for (int j = 0; j < fftype->getNumParams(); j++) {
              llvm::outs() << (j == 0 ? "" : ", ");
              auto pType = fftype->getParamTypes()[j];
              llvm::outs() << pType.getAsString();
            }
            llvm::outs() << ")";
          }
        } else { // not a function pointer param
          llvm::outs() << paramType.getAsString() << " " << paramName;
        }
      }
      llvm::outs() << ")\n";
    }

    // -----------------------------------------------------------------------
    // body
    // -----------------------------------------------------------------------
    std::string paramNamesStr = "";
    if (paramNames.size() > 0)
      paramNamesStr = std::accumulate(
          std::next(paramNames.begin()), paramNames.end(), paramNames[0],
          [](std::string a, std::string b) { return a + "," + b; });

    if (returnTypeName.find("void") != std::string::npos &&
        !returnType->isPointerType()) {
      llvm::outs() << "{\n"                                                //
                   << "\t" << countName << "++;\n"                         //
                   << "\tif(" << hookName << ")\n"                         //
                   << "\t\t" << hookName << "(" << paramNamesStr << ");\n" //
                   << "}\n";
    } else {
      llvm::outs() << "{\n"                        //
                   << "\t" << countName << "++;\n" //
                   << "\tif(" << hookName << ")\n" //
                   << "\t\treturn " << hookName << "(" << paramNamesStr
                   << ");\n"                             //
                   << "\treturn " << retvalName << ";\n" //
                   << "}\n";
    }

    return true;
  }
};
} // namespace mockoto
#endif
