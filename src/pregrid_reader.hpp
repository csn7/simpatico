#ifndef SIMPATICO_PREGRID_READER_HPP
#define SIMPATICO_PREGRID_READER_HPP

#include <stdint.h>
#include <vector>
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
      : reader_(in), function_(function), trace_(trace) {}

    void read() {
      while (read_()) {}
    }

  private:
    fortran_reader<> reader_;
    function_type function_;
    std::ostream* trace_;
    context ctx_;

    bool read_() {
      reader_.record_start();
      {
        ctx_.ifv = reader_.read<int32_t>();
        if (reader_.eof()) {
          return false;
        }

        BOOST_ASSERT(ctx_.ifv == 3);

        if (trace_) {
          *trace_
              << "========== record: 1 ==========\n"
              << "ifv: " << ctx_.ifv << "\n";
        }
      }
      reader_.record_ended();

      reader_.record_start();
      {
        ctx_.hdate = reader_.read_string(24);
        ctx_.xcfst = reader_.read<float>();
        ctx_.field = reader_.read_string(9);
        ctx_.units = reader_.read_string(25);
        ctx_.desc  = reader_.read_string(46);
        ctx_.xlvl  = reader_.read<float>();
        ctx_.nx    = reader_.read<int32_t>();
        ctx_.ny    = reader_.read<int32_t>();
        ctx_.iproj = reader_.read<int32_t>();

        if (trace_) {
          *trace_
              << "========== record: 2 ==========\n"
              << "hdate: " << ctx_.hdate << "\n"
              << "xcfst: " << ctx_.xcfst << "\n"
              << "field: " << ctx_.field << "\n"
              << "units: " << ctx_.units << "\n"
              << "desc: "  << ctx_.desc  << "\n"
              << "xlvl: "  << ctx_.xlvl  << "\n"
              << "nx: "    << ctx_.nx    << "\n"
              << "ny: "    << ctx_.ny    << "\n"
              << "iproj: " << ctx_.iproj << "\n";
        }
      }
      reader_.record_ended();

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

        if (trace_) {
          *trace_
              << "========== record: 3 ==========\n"
              << "startlat: " << ctx_.startlat << "\n"
              << "startlon: " << ctx_.startlon << "\n"
              << "deltalat: " << ctx_.deltalat << "\n"
              << "deltalon: " << ctx_.deltalon << "\n"
              << "dx: "       << ctx_.dx       << "\n"
              << "dy: "       << ctx_.dy       << "\n"
              << "xlonc: "    << ctx_.xlonc    << "\n"
              << "truelat1: " << ctx_.truelat1 << "\n"
              << "truelat2: " << ctx_.truelat2 << "\n"
              ;
        }
      }
      reader_.record_ended();

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

  private:
    void read_cylindrical_equidistant_projection_() {
      ctx_.startlat = reader_.read<float>();
      ctx_.startlon = reader_.read<float>();
      ctx_.deltalat = reader_.read<float>();
      ctx_.deltalon = reader_.read<float>();
      ctx_.dx       = 0;
      ctx_.dy       = 0;
      ctx_.xlonc    = 0;
      ctx_.truelat1 = 0;
      ctx_.truelat2 = 0;
    }

    void read_mercator_projection_() {
      ctx_.startlat = reader_.read<float>();
      ctx_.startlon = reader_.read<float>();
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>();
      ctx_.dy       = reader_.read<float>();
      ctx_.xlonc    = 0;
      ctx_.truelat1 = reader_.read<float>();
      ctx_.truelat2 = 0;
    }

    void read_lambert_conformal_projection_() {
      ctx_.startlat = reader_.read<float>();
      ctx_.startlon = reader_.read<float>();
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>();
      ctx_.dy       = reader_.read<float>();
      ctx_.xlonc    = reader_.read<float>();
      ctx_.truelat1 = reader_.read<float>();
      ctx_.truelat2 = reader_.read<float>();
    }

    void read_polar_stereographic_projection_() {
      ctx_.startlat = reader_.read<float>();
      ctx_.startlon = reader_.read<float>();
      ctx_.deltalat = 0;
      ctx_.deltalon = 0;
      ctx_.dx       = reader_.read<float>();
      ctx_.dy       = reader_.read<float>();
      ctx_.xlonc    = 0;
      ctx_.truelat1 = reader_.read<float>();
      ctx_.truelat2 = 0;
    }
  };
}

#endif
