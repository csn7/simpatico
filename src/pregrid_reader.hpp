#ifndef SIMPATICO_PREGRID_READER_HPP
#define SIMPATICO_PREGRID_READER_HPP

#include <stdint.h>
#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "assert.hpp"
#include "fortran_reader.hpp"
#include "pregrid_context.hpp"

namespace simpatico {
  // http://www.mmm.ucar.edu/mm5/documents/MM5_tut_Web_notes/REGRID/regrid.htm#pgfId-121592
  class pregrid_reader : boost::noncopyable {
  public:
    typedef boost::function<
      void (pregrid_context const&, std::vector<float> const&)
    > function_type;

    explicit pregrid_reader(
        std::istream& in, function_type const& function, std::ostream* trace = 0)
      : reader_(in, trace), function_(function) {}

    void read() {
      while (read_()) {}
    }

    std::ostream* trace() const {
      return reader_.trace();
    }

  private:
    fortran_reader<> reader_;
    function_type function_;
    pregrid_context context_;

    bool read_() {
      if (trace()) {
        *trace() << "========== record: 1 ==========\n";
      }

      reader_.record_start();
      {
        context_.ifv = reader_.read<int32_t>("ifv");
        if (reader_.eof()) {
          return false;
        }
        BOOST_ASSERT(context_.ifv == 3);
      }
      reader_.record_ended();

      if (trace()) {
        *trace() << "========== record: 2 ==========\n";
      }

      reader_.record_start();
      {
        context_.hdate = reader_.read_string(24, "hdate");
        context_.xcfst = reader_.read<float>("xcfst");
        context_.field = reader_.read_string(9, "field");
        context_.units = reader_.read_string(25, "units");
        context_.desc  = reader_.read_string(46, "desc");
        context_.xlvl  = reader_.read<float>("xlvl");
        context_.nx    = reader_.read<int32_t>("nx");
        context_.ny    = reader_.read<int32_t>("ny");
        context_.iproj = reader_.read<int32_t>("iproj");
      }
      reader_.record_ended();

      if (trace()) {
        *trace() << "========== record: 3 ==========\n";
      }

      reader_.record_start();
      {
        switch (context_.iproj) {
          case 0:
            read_cylindrical_equidistant_projection_();
            break;
          case 1:
            read_mercator_projection_();
            break;
          case 3:
            read_lambert_conformal_projection_();
            break;
          case 5:
            read_polar_stereographic_projection_();
            break;
          default:
            BOOST_ASSERT(! "Unknown projection");
        }
      }
      reader_.record_ended();

      if (trace()) {
        *trace() << "========== record: 4 ==========\n";
      }

      std::vector<float> slab(context_.nx * context_.ny);
      reader_.record_start();
      {
        for (int32_t x = 0; x < context_.nx; ++x) {
          for (int32_t y = 0; y < context_.ny; ++y) {
            slab[y + x * context_.ny] = reader_.read<float>();
          }
        }
      }
      reader_.record_ended();

      function_(context_, slab);
      return true;
    }

    void read_cylindrical_equidistant_projection_() {
      context_.startlat = reader_.read<float>("startlat");
      context_.startlon = reader_.read<float>("startlon");
      context_.deltalat = reader_.read<float>("deltalat");
      context_.deltalon = reader_.read<float>("deltalon");
      context_.dx       = 0;
      context_.dy       = 0;
      context_.xlonc    = 0;
      context_.truelat1 = 0;
      context_.truelat2 = 0;
    }

    void read_mercator_projection_() {
      context_.startlat = reader_.read<float>("startlat");
      context_.startlon = reader_.read<float>("startlon");
      context_.deltalat = 0;
      context_.deltalon = 0;
      context_.dx       = reader_.read<float>("dx");
      context_.dy       = reader_.read<float>("dy");
      context_.xlonc    = 0;
      context_.truelat1 = reader_.read<float>("truelat1");
      context_.truelat2 = 0;
    }

    void read_lambert_conformal_projection_() {
      context_.startlat = reader_.read<float>("startlat");
      context_.startlon = reader_.read<float>("startlon");
      context_.deltalat = 0;
      context_.deltalon = 0;
      context_.dx       = reader_.read<float>("dx");
      context_.dy       = reader_.read<float>("dy");
      context_.xlonc    = reader_.read<float>("xlonc");
      context_.truelat1 = reader_.read<float>("truelat1");
      context_.truelat2 = reader_.read<float>("truelat2");
    }

    void read_polar_stereographic_projection_() {
      context_.startlat = reader_.read<float>("startlat");
      context_.startlon = reader_.read<float>("startlon");
      context_.deltalat = 0;
      context_.deltalon = 0;
      context_.dx       = reader_.read<float>("dx");
      context_.dy       = reader_.read<float>("dy");
      context_.xlonc    = 0;
      context_.truelat1 = reader_.read<float>("truelat1");
      context_.truelat2 = 0;
    }
  };
}

#endif
