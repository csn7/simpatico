#ifndef SIMPATICO_FORTRAN_WRITER_HPP
#define SIMPATICO_FORTRAN_WRITER_HPP

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "assert.hpp"
#include "writer.hpp"

namespace simpatico {
  // OPEN(..., FORM='UNFORMATTED', ACCESS='SEQUENTIAL')
  template <bool T_is_big_endian = true>
  class fortran_writer : boost::noncopyable {
  public:
    explicit fortran_writer(std::ostream& out)
      : writer_(out) {}

    template <typename T>
    void write(T const& value) {
      writer<T_is_big_endian>(*record_).write(value);
    }

    void write_string(std::string const& value) {
      writer<T_is_big_endian>(*record_).write_string(value);
    }

    void record_start() {
      record_ = boost::make_shared<std::ostringstream>();
    }

    void record_ended() {
      std::string record = record_->str();
      writer_.write<uint32_t>(record.size());
      writer_.write_string(record);
      writer_.write<uint32_t>(record.size());
      record_.reset();
    }

  private:
    writer<T_is_big_endian> writer_;
    boost::shared_ptr<std::ostringstream> record_;
  };
}

#endif
