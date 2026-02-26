#include "clang/Basic/Version.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream> //std::stringstream

#include "ActionFactory.hpp"
#include "RacketGen.hpp"
#include "ChibiGen.hpp"
#include "ChickenGen.hpp"
#include "Config.hpp"
#include "MockGen.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace mockoto;

static cl::OptionCategory optionCategory("Mockoto Options");

static cl::opt<std::string> modeOpt("mode",
                                    cl::desc("Select output mode (c|h|rkt|chibi|chicken)"),
                                    cl::cat(optionCategory));

static cl::opt<bool>
    printSourcePath("print-src",
                    cl::desc("Print source path for each generated code"),
                    cl::cat(optionCategory));
static cl::opt<bool>
    includeStatic("include-static",
                  cl::desc("Include static functions in generated output"),
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
             "\tmockoto file.h -- -I.\n"
             "\n"
             "Version:\n"
             "\tmockoto --version   # clang/libclang version\n"
             "\tmockoto -V\n");

static std::string createIncludeSource(Config &config,
                                       std::vector<std::string> &files) {
  namespace fs = std::filesystem;
  auto base = std::to_string(std::chrono::steady_clock::now()
                                 .time_since_epoch()
                                 .count());
  std::string fname =
      (fs::temp_directory_path() / ("mockoto-" + base + ".c")).string();
  std::ofstream out(fname);
  if (!out.is_open()) {
    llvm::outs() << "Failed to open temporary include source: " << fname
                 << "\n";
    std::exit(1);
  }

  for (auto ifile : files) {
    fs::path includePath(ifile);
    std::string resolvedInclude = includePath.is_absolute()
                                      ? includePath.lexically_normal().string()
                                      : fs::absolute(includePath).lexically_normal().string();
    out << "#include \"" << resolvedInclude << "\"\n";
    config.includeFiles.push_back(ifile);
  }
  out.close();

  return fname;
}

int main(int argc, const char **argv) {
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--") == 0)
      break;
    if (std::strcmp(argv[i], "--version") == 0) {
      llvm::outs() << clang::getClangFullVersion() << "\n";
      return 0;
    }
    if (std::strcmp(argv[i], "-V") == 0) {
      llvm::outs() << "mockoto " << mockoto::version() << "\n";
      return 0;
    }
  }

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
  } else if (modeVal == "chibi") {
    mode = Config::Mode::BIND_CHIBI;
  } else if (modeVal == "chicken") {
    mode = Config::Mode::BIND_CHICKEN;
  } else {
    llvm::outs() << "Unknown mode '" << modeVal << "'\n";
    return 1;
  }

  Config config(ep, mode);
  config.printSourcePath = printSourcePath.getValue();
  config.includeStaticFunctions = includeStatic.getValue();

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
    r = Tool.run(newActionFactory<RacketGenVisitor>(config).get());
  } else if (mode == Config::Mode::BIND_CHIBI) {
    r = Tool.run(newActionFactory<ChibiGenVisitor>(config).get());
  } else if (mode == Config::Mode::BIND_CHICKEN) {
    r = Tool.run(newActionFactory<ChickenGenVisitor>(config).get());
  } else {
    r = Tool.run(newActionFactory<MockGenVisitor>(config).get());
  }

  // cleanup
  if (tmpf != "")
    std::remove(tmpf.c_str());
  return r;
}
