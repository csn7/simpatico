#ifndef SIMPATICO_MSM_READER_HPP
#define SIMPATICO_MSM_READER_HPP

#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/function.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "reader.hpp"
#include "time.hpp"

namespace simpatico {
  class msm_reader : public reader<> {
  public:
    struct context {
      boost::posix::ptime time;
      msm_grid grid;
      std::pair<int, int> type_;
      float r;
      int16_t e;
      int16_t d;
    };

    typedef boost::function2<
      void, context const&, std::vector<double> const&
    > function_type;

    explicit msm_reader(
        std::istream& in, function_type function, bool trace = false)
      : reader<>(in), function_(function), trace_(trace) {}

    void read_message() {
      std::streampos message_size = read_indicator_section_() - 4;
      while (tell() < message_size) {
        read_section_();
      }
      read_end_section_();
    }

  private:
    function_type function_;
    bool trace_;
    uint32_t section_size_;
    context ctx_;

    uint64_t read_indicator_section_() {
      std::string code = read_string(4);
      BOOST_ASSERT(code == "GRIB");
      seek(4);
      uint64_t message_size = read<uint64_t>();

      if (trace_) {
        std::cerr
            << "========== section:0 ==========\n"
            << "message_size: " << message_size << "\n";
      }

      return message_size;
    }

    void read_section_() {
      section_size_ = read<uint32_t>();
      int section_number = read<uint8_t>();

      if (trace_) {
        std::cerr
            << "========== section:" << section_number << " ==========\n"
            << "section_size: " << section_size_ << "\n";
      }

      switch (section_number) {
        case 1:
          read_identification_section_();
          break;
        case 3:
          read_grid_definition_section_();
          break;
        case 4:
          read_product_definition_section_();
          break;
        case 5:
          read_data_representation_section_();
          break;
        case 7:
          read_data_section_();
          break;
        default:
          if (trace_) {
            std::cerr << "IGNORE UNKNOWN SECTION\n";
          }
          seek(section_size_ - 5);
      }
    }

    void read_identification_section_() {
      seek(7);
      // reference time (UTC)
      struct tm t;
      memset(&t, 0, sizeof(t));
      t.tm_year = read<uint16_t>() - 1900;
      t.tm_mon  = read<uint8_t>() - 1;
      t.tm_mday = read<uint8_t>();
      t.tm_hour = read<uint8_t>();
      t.tm_min  = read<uint8_t>();
      t.tm_sec  = read<uint8_t>();
      ctx_.time = boost::posix_time::date_from_tm(t);
      seek(section_size_ - 19);

      if (trace_) {
        std::cerr << "reference_time: " << ctx_.time << "\n";
      }
    }

    void read_grid_definition_section_() {
      int earth_shape = read<uint8_t>();
      seek(6);
      int template_number = read<uint16_t>();

      if (trace_) {
        std::cerr
            << "earth_shape: " << earth_shape << "\n"
            << "---------- template:" << template_number << " ----------\n";
      }

      switch (template_number) {
        case 0:
          read_grid_definition_template0_();
          break;
        default:
          if (trace_) {
            std::cerr << "IGNORE UNKNOWN TEMPLATE\n";
          }
          seek(section_size_ - 14);
      }
    }

    void read_grid_definition_template0_() {
      seek(16);
      ctx_.grid.x.n = read<uint32_t>(); // n_i
      ctx_.grid.y.n = read<uint32_t>(); // n_j
      seek(8);
      ctx_.grid.y.v0 = read<uint32_t>() * 0.000001; // la_1
      ctx_.grid.x.v0 = read<uint32_t>() * 0.000001; // lo_1
      seek(1);
      ctx_.grid.y.v1 = read<uint32_t>() * 0.000001; // la_2
      ctx_.grid.x.v1 = read<uint32_t>() * 0.000001; // lo_2
      seek(section_size_ - 79);

      if (trace_) {
        std::cerr
            << "n_i: "  << ctx_.grid.x.n  << "\n"
            << "n_j: "  << ctx_.grid.y.n  << "\n"
            << "la_1: " << ctx_.grid.y.v0 << "\n"
            << "lo_1: " << ctx_.grid.x.v0 << "\n"
            << "la_2: " << ctx_.grid.y.v1 << "\n"
            << "lo_2: " << ctx_.grid.y.v0 << "\n";
      }
    }

    void read_product_definition_section_() {
      seek(2);
      int template_number = read<uint16_t>();

      if (trace_) {
        std::cerr << "---------- template:" << template_number << " ----------\n";
      }

      switch (template_number) {
        case 0:
        case 8:
          read_product_definition_template0_or_template8_();
          break;
        default:
          if (trace_) {
            std::cerr << "IGNORE UNKNOWN TEMPLATE\n";
          }
          seek(section_size_ - 9);
      }
    }

    void read_product_definition_template0_or_template8_() {
      ctx_.parameter_category = read<uint8_t>();
      ctx_.parameter_number = read<uint8_t>();
      seek(section_size_ - 45);

      if (trace_) {
        std::cerr
          << "parameter_category: " << ctx_.parameter_category << "\n"
          << "parameter_number: "   << ctx_.parameter_number   << "\n";
          ;
      }

      // (3,1) => Pressure reduced to MSL [Pa]
      // (3,0) => Pressure [Pa]
      // (2,2) => u-component of wind [m/s]
      // (2,3) => v-component of wind [m/s]
      // (0,0) => Temperature [K]
      // (1,1) => Relative humidity [%]
      // (6,3) => Low cloud cover [%]
      // (6,4) => Medium cloud cover [%]
      // (6,5) => High cloud cover [%]
      // (6,1) => Total cloud cover [%]
      // (1,8) => Total precipitation [kg/m^2]

      // (3,5) => Geopotential height [gpm]
      // (2,2) => u-component of wind [m/s]
      // (2,3) => v-component of wind [m/s]
      // (0,0) => Temperature [K]
      // (2,8) => Vertical velocity (pressure) [Pa/s]
      // (1,1) => Relative humidity [%]
    }

    void read_data_representation_section_() {
      seek(4);
      int template_number = read<uint16_t>();

      if (trace_) {
        std::cerr << "---------- template:" << template_number << " ----------\n";
      }

      switch (template_number) {
        case 0:
          read_data_representation_template0_();
          break;
        default:
          if (trace_) {
            std::cerr << "IGNORE UNKNOWN TEMPLATE\n";
          }
          seek(section_size_ - 11);
      }
    }

    void read_data_representation_template0_() {
      ctx_.r = read<float>();
      ctx_.e = read<int16_t>();
      ctx_.d = read<int16_t>();
      int bits = read<uint8_t>();
      BOOST_ASSERT(bits == 12);
      seek(section_size_ - 20);

      if (trace_) {
        std::cerr
            << "r: " << ctx_.r << "\n"
            << "e: " << ctx_.e << "\n"
            << "d: " << ctx_.d << "\n";
      }
    }

    void read_data_section_() {
      std::vector<uint8_t> buffer(section_size_ - 5);
      read_buffer(buffer);
      if (trace_) {
        std::cerr << "buffer_size: " << buffer.size() << "\n";
      }

      std::vector<double> data(ctx_.n_i * ctx_.n_j);
      for (int j = 0; j < int(ctx_.n_j); ++j) {
        for (int i = 0; i < int(ctx_.n_i); ++i) {
          // see read_data_representation_template0_
          int bit_index = (i + j * ctx_.n_i) * 12;
          double value;
          if (bit_index % 8 == 0) {
            value = buffer[bit_index / 8] << 4 | buffer[bit_index / 8 + 1] >> 4;
          } else {
            value = (buffer[bit_index / 8] & 0x0F) << 8 | buffer[bit_index / 8 + 1];
          }
          data[i + j * ctx_.n_i]
            = (ctx_.r + value * pow(2, ctx_.e)) / pow(10, ctx_.d);
        }
      }

      function_(ctx_, data);
    }

    void read_end_section_() {
      std::string code = read_string(4);
      BOOST_ASSERT(code == "7777");
    }
  };
}

#endif
