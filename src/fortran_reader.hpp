#ifndef SIMPATICO_FORTRAN_READER_HPP
#define SIMPATICO_FORTRAN_READER_HPP

#include <stdint.h>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include "reader.hpp"

namespace simpatico {
  // OPEN(..., FORM='UNFORMATTED', ACCESS='SEQUENTIAL')
  template <bool T_is_big_endian = true>
  class fortran_reader : boost::noncopyable {
  public:
    explicit fortran_reader(std::istream& in)
      : reader_(in), size_(), position_() {}

    template <typename T>
    T read() {
      return reader_.template read<T>();
    }

    std::string read_string(size_t size) {
      return reader_.read_string(size);
    }

    bool eof() const {
      return reader_.eof();
    }

    void record_start() {
      size_     = reader_.template read<uint32_t>();
      position_ = reader_.position();
    }

    void record_ended() {
      BOOST_ASSERT(reader_.position() - position_ == size_);
      uint32_t const size = reader_.template read<uint32_t>();
      BOOST_ASSERT(size_ == size);
    }

  private:
    reader<T_is_big_endian> reader_;
    uint32_t size_;
    std::streampos position_;
  };
}

#endif
