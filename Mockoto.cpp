#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream> //std::stringstream

#include "ActionFactory.hpp"
#include "Bindgen.hpp"
#include "Config.hpp"
#include "Mockgen.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace mockoto;

static cl::OptionCategory optionCategory("Mockoto Options");

static cl::opt<std::string> modeOpt("mode",
                                    cl::desc("Select output mode (c|h|rkt)"),
                                    cl::cat(optionCategory));

static cl::opt<bool>
    printSourcePath("print-src",
                    cl::desc("Print source path for each generated code"),
                    cl::cat(optionCategory));

static cl::list<std::string>
    pathExclude("path-exclude", cl::desc("Exclude files with given substring"),
                cl::cat(optionCategory));
static cl::list<std::string>
    pathInclude("path-include", cl::desc("Include path as in #include"),
                cl::cat(optionCategory));

static cl::extrahelp
    moreHelp("\n" //
             "To pass options to clang use `--`, for example,\n"
             "\tmockoto file.h -- -I.\n");

static std::string createIncludeSource(Config &config,
                                       std::vector<std::string> &files) {
  std::string fname = "/tmp/mockoto.c";
  std::ofstream out(fname);

  for (auto ifile : files) {
    out << "#include \"" << ifile << "\"\n";
    config.includeFiles.push_back(ifile);
  }
  out.close();

  return fname;
}

int main(int argc, const char **argv) {
  auto OptionsParser =
      CommonOptionsParser::create(argc, argv, optionCategory, cl::ZeroOrMore);

  // prepare exclude patterns
  std::list<std::string> ep;
  for (auto e : pathExclude)
    ep.push_back(e);

  // select mode
  Config::Mode mode;
  std::string modeVal = modeOpt.getValue();
  if (modeVal == "" || modeVal == "c" || modeVal == "C") {
    mode = Config::Mode::MOCK_C;
  } else if (modeVal == "h" || modeVal == "H") {
    mode = Config::Mode::MOCK_H;
  } else if (modeVal == "rkt") {
    mode = Config::Mode::BIND_RKT;
  } else {
    llvm::outs() << "Unknown mode '" << modeVal << "'\n";
    return 1;
  }

  Config config(ep, mode);
  config.printSourcePath = printSourcePath.getValue();

  // prepare include files
  std::vector<std::string> pathList = OptionsParser->getSourcePathList();
  std::string tmpf;
  tmpf = createIncludeSource(config, pathList);
  pathList.clear();
  pathList.push_back(tmpf);

  ClangTool Tool(OptionsParser->getCompilations(), pathList);

  // select right action
  int r = 0;
  if (mode == Config::Mode::BIND_RKT) {
    r = Tool.run(newActionFactory<BindgenVisitor>(config).get());
  } else {
    r = Tool.run(newActionFactory<MockgenVisitor>(config).get());
  }

  // cleanup
  if (tmpf != "")
    std::remove(tmpf.c_str());
  return r;
}
