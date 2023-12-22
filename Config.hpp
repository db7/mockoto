#ifndef MOCKOTO_CONFIG_HPP
#define MOCKOTO_CONFIG_HPP

#include <list>
#include <string>

namespace mockoto {

class Config {
public:
  enum Mode {
    MOCK_C,
    MOCK_H,
    BIND_RKT,
  } mode;

  Config(std::list<std::string> excludePatterns, Mode mode)
      : excludePatterns(excludePatterns), mode(mode) {}

  const std::list<std::string> excludePatterns;
  std::list<std::string> includeFiles;

  bool printSourcePath; //< print source path of each generated mock or binding
};

} // namespace mockoto

#endif
