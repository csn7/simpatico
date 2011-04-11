#ifndef SIMPATICO_ASSERT_HPP
#define SIMPATICO_ASSERT_HPP

#include <sstream>
#include <stdexcept>

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/assert.hpp>

namespace boost {
  inline void assertion_failed(
      char const* expression,
      char const* function,
      char const* file,
      long line) {
    std::ostringstream out;
    out << "Assertion failed: "
        << expression
        << " in function "
        << function
        << " at file "
        << file
        << " line "
        << line;
    throw std::runtime_error(out.str());
  }
}

#endif
