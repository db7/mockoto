#ifndef MOCKOTO_FACTORY_HPP
#define MOCKOTO_FACTORY_HPP

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

#include "Config.hpp"

namespace mockoto {

template <typename V> class Consumer : public clang::ASTConsumer {
public:
  explicit Consumer(clang::ASTContext *Context, Config Config,
                    llvm::StringRef InFile)
      : Visitor(Context, Config, InFile) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  V Visitor;
};

template <typename V> class Action : public clang::ASTFrontendAction {
  Config Config;

public:
  Action(mockoto::Config Config) : Config(Config) {}

protected:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<Consumer<V>>(&Compiler.getASTContext(), Config,
                                         InFile);
  }
};

template <typename V>
std::unique_ptr<clang::tooling::FrontendActionFactory>
newActionFactory(Config Config) {

  class actionFactory : public clang::tooling::FrontendActionFactory {
    mockoto::Config Config;

  public:
    actionFactory(mockoto::Config &cfg) : Config(cfg) {}

    std::unique_ptr<clang::FrontendAction> create() override {
      return std::unique_ptr<Action<V>>(new Action<V>(Config));
    }
  };

  return std::unique_ptr<clang::tooling::FrontendActionFactory>(
      new actionFactory(Config));
}

} // namespace mockoto
#endif
