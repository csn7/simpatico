#ifndef SIMPATICO_READER_HPP
#define SIMPATICO_READER_HPP

#include <iostream>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_fundamental.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include "endian.hpp"
#include "unsafe_cast.hpp"

namespace simpatico {
  template <bool T_is_big_endian = true>
  class reader : boost::noncopyable {
  public:
    explicit reader(std::istream& in)
      : in_(in) {}

    template <typename T>
    T read() {
      BOOST_STATIC_ASSERT(boost::is_fundamental<T>::value);
      unsafe_cast<T> cast;
      in_.read(cast.data(), cast.size());
      endian<T_is_big_endian>::swap(cast.begin(), cast.end());
      return cast.get();
    }

    template <typename T>
    T read_1s_complement() {
      BOOST_STATIC_ASSERT(boost::is_signed<T>::value);
      typedef typename boost::make_unsigned<T>::type unsigned_type;
      static unsigned_type const msb = 1 << sizeof(unsigned_type) * 8 - 1;
      unsigned_type const value = read<unsigned_type>();
      if (value & msb) {
        return -(value ^ msb);
      } else {
        return value;
      }
    }

    template <typename T>
    void read_buffer(std::vector<T>& buffer) {
      BOOST_STATIC_ASSERT(sizeof(T) == 1);
      in_.read(reinterpret_cast<char*>(&buffer[0]), buffer.size());
    }

    std::string read_string(size_t size) {
      std::vector<char> buffer(size);
      read_buffer(buffer);
      return std::string(buffer.begin(), buffer.end());
    }

    bool eof() const {
      return in_.eof();
    }

    std::streampos position() const {
      return in_.tellg();
    }

    void skip(std::streampos offset) {
      in_.seekg(offset, std::ios::cur);
    }

  private:
    std::istream& in_;
  };
}

#endif
