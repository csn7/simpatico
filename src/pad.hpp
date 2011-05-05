#ifndef SIMAPTICO_PAD_HPP
#define SIMAPTICO_PAD_HPP

#include <stddef.h>
#include <string>

namespace simpatico {
  inline std::string pad(
      std::string source, size_t size, std::string const& padding = " ") {
    while (source.size() < size) {
      source += padding;
    }
    return source;
  }
}

#endif
