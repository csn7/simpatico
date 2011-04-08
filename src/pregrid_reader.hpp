#ifndef SIMPATICO_PREGRID_READER_HPP
#define SIMPATICO_PREGRID_READER_HPP

#include <stdint.h>
#include <vector>
#include <boost/assert.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "fortran_reader.hpp"

namespace simpatico {
  // http://www.mmm.ucar.edu/mm5/documents/MM5_tut_Web_notes/REGRID/regrid.htm#pgfId-121592
  class pregrid_reader : boost::noncopyable {
  public:
    struct context {
      int32_t     ifv;
      std::string hdate;
      float       xcfst;
      std::string field;
      std::string units;
      std::string desc;
      float       xlvl;
      int32_t     nx;
      int32_t     ny;
      int32_t     iproj;
      float       startlat;
      float       startlon;
      float       deltalat;
      float       deltalon;
      float       dx;
      float       dy;
      float       xlonc;
      float       truelat1;
      float       truelat2;
    };

    typedef boost::function<
      void (context const&, std::vector<float> const&)
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
    context ctx_;

    bool read_() {
      if (trace()) {
        *trace() << "========== record: 1 ==========\n";
      }

      reader_.record_start();
      {
        ctx_.ifv = reader_.read<int32_t>("ifv");
        if (reader_.eof()) {
          return false;
        }
        BOOST_ASSERT(ctx_.ifv == 3);
      }
      reader_.record_ended();

      if (trace()) {
        *trace() << "========== record: 2 ==========\n";
      }

      reader_.record_start();
      {
        ctx_.hdate = reader_.read_string(24, "hdate");
        ctx_.xcfst = reader_.read<float>("xcfst");
        ctx_.field = reader_.read_string(9, "field");
        ctx_.units = reader_.read_string(25, "units");
        ctx_.desc  = reader_.read_string(46, "desc");
        ctx_.xlvl  = reader_.read<float>("xlvl");
        ctx_.nx    = reader_.read<int32_t>("nx");
        ctx_.ny    = reader_.read<int32_t>("ny");
        ctx_.iproj = reader_.read<int32_t>("iproj");
      }
      reader_.record_ended();

      if (trace()) {
        *trace() << "========== record: 3 ==========\n";
      }

      reader_.record_start();
      {
        switch (ctx_.iproj) {
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

      std::vector<float> slab(ctx_.nx * ctx_.ny);
      reader_.record_start();
      {
        for (int32_t x = 0; x < ctx_.nx; ++x) {
          for (int32_t y = 0; y < ctx_.ny; ++y) {
            slab[y + x * ctx_.ny] = reader_.read<float>();
          }
        }
      }
      reader_.record_ended();

      function_(ctx_, slab);
      return true;
    }

    void read_cylindrical_equidistant_projection_() {
      ctx_.startlat = reader_.read<float>("startlat");
      ctx_.startlon = reader_.read<float>("startlon");
      ctx_.deltalat = reader_.read<float>("deltalat");
      ctx_.deltalon = reader_.read<float>("deltalon");
      ctx_.dx       = 0;
      ctx_.dy       = 0;
      ctx_.xlonc    = 0;
      ctx_.truelat1 = 0;
      ctx_.truelat2 = 0;
    }

    void read_mercator_projection_() {
      ctx_.startlat = reader_.read<float>("startlat");
      ctx_.startlon = reader_.read<float>("startlon");
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>("dx");
      ctx_.dy       = reader_.read<float>("dy");
      ctx_.xlonc    = 0;
      ctx_.truelat1 = reader_.read<float>("truelat1");
      ctx_.truelat2 = 0;
    }

    void read_lambert_conformal_projection_() {
      ctx_.startlat = reader_.read<float>("startlat");
      ctx_.startlon = reader_.read<float>("startlon");
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>("dx");
      ctx_.dy       = reader_.read<float>("dy");
      ctx_.xlonc    = reader_.read<float>("xlonc");
      ctx_.truelat1 = reader_.read<float>("truelat1");
      ctx_.truelat2 = reader_.read<float>("truelat2");
    }

    void read_polar_stereographic_projection_() {
      ctx_.startlat = reader_.read<float>("startlat");
      ctx_.startlon = reader_.read<float>("startlon");
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>("dx");
      ctx_.dy       = reader_.read<float>("dy");
      ctx_.xlonc    = 0;
      ctx_.truelat1 = reader_.read<float>("truelat1");
      ctx_.truelat2 = 0;
    }
  };
}

#endif
