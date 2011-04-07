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
    explicit fortran_reader(std::istream& in, std::ostream* trace = 0)
      : reader_(in, trace), size_(), position_() {}

    template <typename T>
    T read(char const* name = 0) {
      return reader_.template read<T>(name);
    }

    std::string read_string(size_t size, char const* name = 0) {
      return reader_.read_string(size, name);
    }

    bool eof() const {
      return reader_.eof();
    }

    std::ostream* trace() const {
      return reader_.trace();
    }

    void record_start() {
      size_     = reader_.template read<uint32_t>();
      position_ = reader_.position();

      if (trace()) {
        *trace() << "| [record_start] " << size_ << "\n";
      }
    }

    void record_ended() {
      BOOST_ASSERT(reader_.position() - position_ == size_);
      uint32_t const size = reader_.template read<uint32_t>();
      BOOST_ASSERT(size_ == size);

      if (trace()) {
        *trace() << "| [record_ended] " << size << "\n";
      }
    }

  private:
    reader<T_is_big_endian> reader_;
    uint32_t size_;
    std::streampos position_;
  };
}

#endif
