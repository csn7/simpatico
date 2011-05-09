#ifndef SIMPATICO_READER_HPP
#define SIMPATICO_READER_HPP

#include <stddef.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <boost/iostreams/operations.hpp>
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
    explicit reader(std::istream& in, std::ostream* trace = 0)
      : in_(in), trace_(trace), position_(), eof_() {}

    template <typename T>
    T read(char const* name = 0) {
      BOOST_STATIC_ASSERT(boost::is_fundamental<T>::value);
      unsafe_cast<T> cast;
      read_(cast.data(), cast.size());
      endian<T_is_big_endian>::swap(cast.begin(), cast.end());

      if (trace_ && name) {
        if (sizeof(T) == 1) {
          *trace_ << "| " << name << ": " << int16_t(cast.get()) << "\n";
        } else {
          *trace_ << "| " << name << ": " << cast.get() << "\n";
        }
      }
      return cast.get();
    }

    template <typename T>
    T read_1s_complement(char const* name = 0) {
      BOOST_STATIC_ASSERT(boost::is_signed<T>::value);
      typedef typename boost::make_unsigned<T>::type unsigned_type;
      static unsigned_type const msb = 1 << sizeof(unsigned_type) * 8 - 1;

      unsigned_type const uvalue = read<unsigned_type>();
      T value;
      if (uvalue & msb) {
        value = -(uvalue ^ msb);
      } else {
        value = uvalue;
      }

      if (trace_ && name) {
        if (sizeof(T) == 1) {
          *trace_ << "| " << name << ": " << int16_t(value) << "\n";
        } else {
          *trace_ << "| " << name << ": " << value << "\n";
        }
      }
      return value;
    }

    template <typename T>
    void read_buffer(std::vector<T>& buffer) {
      BOOST_STATIC_ASSERT(sizeof(T) == 1);
      read_(reinterpret_cast<char*>(&buffer[0]), buffer.size());
    }

    std::string read_string(size_t size, char const* name = 0) {
      std::vector<char> buffer(size);
      read_buffer(buffer);
      std::string value(buffer.begin(), buffer.end());

      if (trace_ && name) {
        *trace_ << "| " << name << ": " << value << "\n";
      }
      return value;
    }

    bool eof() const {
      return eof_ || in_.eof();
    }

    size_t position() const {
      // can not use tellg...
      return position_;
    }

    void skip(size_t size) {
      // can not use seekg...
      if (size > 0) {
        for (size_t i = 0; i < size; ++i) {
          char c = 0;
          read_(&c, 1);
        }
      }

      if (trace_) {
        *trace_ << "| [skip] " << size << "\n";
      }
    }

    std::ostream* trace() const {
      return trace_;
    }

  private:
    std::istream& in_;
    std::ostream* trace_;
    size_t position_;
    bool eof_;

    void read_(char* buffer, size_t size) {
      std::streamsize read_size = boost::iostreams::read(in_, buffer, size);
      if (read_size > 0) {
        position_ += read_size;
      } else {
        eof_ = true;
      }
    }
  };
}

#endif
