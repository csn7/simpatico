#ifndef SIMPATICO_PREGRID_WRITER_HPP
#define SIMPATICO_PREGRID_WRITER_HPP

#include <iostream>
#include <string>
#include <boost/noncopyable.hpp>
#include "assert.hpp"
#include "fortran_writer.hpp"
#include "pad.hpp"
#include "pregrid_context.hpp"

namespace simpatico {
  class pregrid_writer : boost::noncopyable {
  public:
    explicit pregrid_writer(std::ostream& out)
      : writer_(out) {}

    template <typename T>
    void write(T const& value) {
      writer_.write(value);
    }

    void write_string(std::string const& value) {
      writer_.write_string(value);
    }

    void record_start() {
      writer_.record_start();
    }

    void record_ended() {
      writer_.record_ended();
    }

    void write_context(pregrid_context const& context) {
      writer_.record_start();
      {
        writer_.write<int32_t>(context.ifv);
      }
      writer_.record_ended();

      writer_.record_start();
      {
        writer_.write_string(pad(context.hdate, 24));
        writer_.write(context.xcfst);
        writer_.write_string(pad(context.field, 9));
        writer_.write_string(pad(context.units, 25));
        writer_.write_string(pad(context.desc, 46));
        writer_.write(context.xlvl);
        writer_.write(context.nx);
        writer_.write(context.ny);
        writer_.write(context.iproj);
      }
      writer_.record_ended();

      writer_.record_start();
      switch (context.iproj) {
        case 0:
          write_cylindrical_equidistant_projection_(context);
          break;
        case 1:
          write_mercator_projection_(context);
          break;
        case 3:
          write_lambert_conformal_projection_(context);
          break;
        case 5:
          write_polar_stereographic_projection_(context);
          break;
        default:
          BOOST_ASSERT(! "Unknown projection");
      }
      writer_.record_ended();
    }

  private:
    fortran_writer<> writer_;

    void write_cylindrical_equidistant_projection_(pregrid_context const& context) {
      writer_.write<float>(context.startlat);
      writer_.write<float>(context.startlon);
      writer_.write<float>(context.deltalat);
      writer_.write<float>(context.deltalon);
    }

    void write_mercator_projection_(pregrid_context const& context) {
      writer_.write<float>(context.startlat);
      writer_.write<float>(context.startlon);
      writer_.write<float>(context.dx);
      writer_.write<float>(context.dy);
      writer_.write<float>(context.truelat1);
    }

    void write_lambert_conformal_projection_(pregrid_context const& context) {
      writer_.write<float>(context.startlat);
      writer_.write<float>(context.startlon);
      writer_.write<float>(context.dx);
      writer_.write<float>(context.dy);
      writer_.write<float>(context.xlonc);
      writer_.write<float>(context.truelat1);
      writer_.write<float>(context.truelat2);
    }

    void write_polar_stereographic_projection_(pregrid_context const& context) {
      writer_.write<float>(context.startlat);
      writer_.write<float>(context.startlon);
      writer_.write<float>(context.dx);
      writer_.write<float>(context.dy);
      writer_.write<float>(context.xlonc);
      writer_.write<float>(context.truelat1);
    }
  };
}

#endif
