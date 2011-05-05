#ifndef SIMPATICO_WRITER_HPP
#define SIMPATICO_WRITER_HPP

#include <stddef.h>
#include <iostream>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include "endian.hpp"
#include "unsafe_cast.hpp"

namespace simpatico {
  template <bool T_is_big_endian = true>
  class writer : boost::noncopyable {
  public:
    explicit writer(std::ostream& out)
      : out_(out) {}

    template <typename T>
    void write(T const& value) {
      BOOST_STATIC_ASSERT(boost::is_fundamental<T>::value);
      unsafe_cast<T> cast(value);
      endian<T_is_big_endian>::swap(cast.begin(), cast.end());
      write_(cast.data(), cast.size());
    }

    void write_string(std::string const& value) {
      write_(value.data(), value.size());
    }

  private:
    std::ostream& out_;

    void write_(char const* buffer, size_t size) {
      out_.write(buffer, size);
    }
  };
}

#endif
