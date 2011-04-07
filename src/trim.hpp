#ifndef SIMPATICO_TRIM_HPP
#define SIMPATICO_TRIM_HPP

#include <string>

namespace simpatico {
  inline std::string trim(std::string const& source) {
    std::string::size_type const start = source.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
      return std::string();
    }

    std::string::size_type const ended = source.find_last_not_of(" \t\r\n");
    if (ended == std::string::npos) {
      return std::string();
    }
    return source.substr(start, ended - start + 1);
  }
}

#endif
