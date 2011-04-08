#ifndef SIMPATICO_MSM_READER_HPP
#define SIMPATICO_MSM_READER_HPP

#include <math.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <boost/assert.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "reader.hpp"

namespace simpatico {
  // http://www.wmo.int/pages/prog/www/WMOCodes/Guides/GRIB/GRIB2_062006.pdf
  class msm_reader : boost::noncopyable {
  public:
    struct context {
      struct tm reference_time;
      uint32_t n_i;
      uint32_t n_j;
      uint32_t la_1;
      uint32_t lo_1;
      uint32_t la_2;
      uint32_t lo_2;
      uint32_t d_i;
      uint32_t d_j;
      int parameter_category;
      int parameter_number;
      float r;
      int16_t e;
      int16_t d;
    };

    typedef boost::function<
      void (context const&, std::vector<double> const& data)
    > function_type;

    explicit msm_reader(
        std::istream& in, function_type const& function, std::ostream* trace = 0)
      : reader_(in, trace), function_(function) {}

    void read() {
      std::streampos size = read_indicator_section_() - 4;
      while (reader_.position() < size) {
        read_section_();
      }
      read_end_section_();
    }

    std::ostream* trace() const {
      return reader_.trace();
    }

  private:
    reader<> reader_;
    function_type function_;
    uint32_t section_size_;
    int data_representation_template_;
    context ctx_;

    uint64_t read_indicator_section_() {
      if (trace()) {
        *trace() << "========== indicator section ==========\n";
      }

      std::string code = reader_.read_string(4, "\"GRIB\"");
      BOOST_ASSERT(code == "GRIB");

      reader_.skip(2);
      reader_.read<uint8_t>("Discipline - GRIB Master Table Number");
      reader_.read<uint8_t>("GRIB Edition Number");
      return reader_.read<uint64_t>("Total length of GRIB message in octets");
    }

    void read_section_() {
      if (trace()) {
        *trace() << "========== section ==========\n";
      }

      section_size_ = reader_.read<uint32_t>("Length of section in octets");
      int section_number = reader_.read<uint8_t>("Number of section");

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
        case 6:
          read_bitmap_section_();
          break;
        case 7:
          read_data_section_();
          break;
        default:
          if (trace()) {
            *trace() << "!!! IGNORE UNKNOWN SECTION !!!\n";
          }
          reader_.skip(section_size_ - 5);
      }
    }

    void read_end_section_() {
      if (trace()) {
        *trace() << "========== end section ==========\n";
      }

      std::string code = reader_.read_string(4, "\"7777\"");
      BOOST_ASSERT(code == "7777");
    }

    // Section 1
    void read_identification_section_() {
      reader_.read<uint16_t>("Identification of originating/generating centre");
      reader_.read<uint16_t>("Identification of originating/generating sub-centre");
      reader_.read<uint8_t>("GRIB Master Tables Version Number");
      reader_.read<uint8_t>("Version number of GRIB Local Tables used to augment Master Tables");
      reader_.read<uint8_t>("Significance of Reference Time");

      memset(&ctx_.reference_time, sizeof(ctx_.reference_time), 0);
      ctx_.reference_time.tm_year = reader_.read<uint16_t>("Year") - 1900;
      ctx_.reference_time.tm_mon  = reader_.read<uint8_t>("Month") - 1;
      ctx_.reference_time.tm_mday = reader_.read<uint8_t>("Day");
      ctx_.reference_time.tm_hour = reader_.read<uint8_t>("Hour");
      ctx_.reference_time.tm_min  = reader_.read<uint8_t>("Minute");
      ctx_.reference_time.tm_sec  = reader_.read<uint8_t>("Second");

      reader_.read<uint8_t>("Production status of processed data in this GRIB message");
      reader_.read<uint8_t>("Type of processed data in this GRIB message");
      reader_.skip(section_size_ - 21);
    }

    // Section 3
    void read_grid_definition_section_() {
      reader_.read<uint8_t>("Source of grid definition");
      reader_.read<uint32_t>("Number of data points");
      reader_.read<uint8_t>("Number of octets for optional list of numbers defining number of points");
      reader_.read<uint8_t>("Interpretation of list of numbers defining number of points");

      int template_number = reader_.read<uint16_t>("Grid Definition Template Number");
      switch (template_number) {
        case 0:
          read_grid_definition_template0_();
          break;
        default:
          if (trace()) {
            *trace() << "!!! IGNORE UNKNOWN TEMPLATE !!!\n";
          }
          reader_.skip(section_size_ - 14);
      }
    }

    // Section 3.0
    void read_grid_definition_template0_() {
      reader_.read<uint8_t>("Shape of the earth");
      reader_.read<uint8_t>("Scale factor of radius of spherical earth");
      reader_.read<uint32_t>("Scaled value of radius of spherical earth");
      reader_.read<uint8_t>("Scale factor of major axis of oblate spheroid earth");
      reader_.read<uint32_t>("Scaled value of major axis of oblate spheroid earth");
      reader_.read<uint8_t>("Scale factor of minor axis of oblate spheroid earth");
      reader_.read<uint32_t>("Scaled value of minor axis of oblate spheroid earth");
      ctx_.n_i = reader_.read<uint32_t>("Ni - number of points along a parallel");
      ctx_.n_j = reader_.read<uint32_t>("Nj - number of points along a meridian");
      reader_.read<uint32_t>("Basic angle of the initial production domain");
      reader_.read<uint32_t>("Subdivisions of basic angle used to define extreme longitudes and latitudes, and direction increments");
      ctx_.la_1 = reader_.read<uint32_t>("La1 - latitude of first grid point");
      ctx_.lo_1 = reader_.read<uint32_t>("Lo1 - longitude of first grid point");
      reader_.read<uint8_t>("Resolution and component flags");
      ctx_.la_2 = reader_.read<uint32_t>("La2 - latitude of last grid point");
      ctx_.lo_2 = reader_.read<uint32_t>("Lo2 - longitude of last grid point");
      ctx_.d_i = reader_.read<uint32_t>("Di - i direction increment");
      ctx_.d_j = reader_.read<uint32_t>("Dj - j direction increment");
      reader_.read<uint8_t>("Scanning mode");
      reader_.skip(section_size_ - 72);
    }

    // Section 4
    void read_product_definition_section_() {
      reader_.read<uint16_t>("Number of coordinates values after Template");
      int template_number = reader_.read<uint16_t>("Product Definition Template Number");

      switch (template_number) {
        case 0:
          read_product_definition_template0_();
          break;
        case 8:
          read_product_definition_template8_();
          break;
        default:
          if (trace()) {
            *trace() << "!!! IGNORE UNKNOWN TEMPLATE !!!\n";
          }
          reader_.skip(section_size_ - 9);
      }
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

    void read_product_definition_template0_or_template8_() {
      ctx_.parameter_category = reader_.read<uint8_t>("Parameter category");
      ctx_.parameter_number = reader_.read<uint8_t>("Parameter number");
      reader_.read<uint8_t>("Type of generating process");
      reader_.read<uint8_t>("Background generating process identifier");
      reader_.read<uint8_t>("Analysis or forecast generating processes identifier");
      reader_.read<uint16_t>("Hours of observational data cutoff after reference time");
      reader_.read<uint8_t>("Minutes of observational data cutoff after reference time");
      reader_.read<uint8_t>("Indicator of unit of time range");
      reader_.read<uint32_t>("Forecast time in units defined by octet 18");
      reader_.read<uint32_t>("Type of first fixed surface");
      reader_.read<uint8_t>("Scale factor of first fixed surface");
      reader_.read<uint8_t>("Scaled value of first fixed surface");
      reader_.read<uint32_t>("Type of second fixed surface");
      reader_.read<uint8_t>("Scale factor of second fixed surface");
      reader_.read<uint8_t>("Scaled value of second fixed surface");
    }

    // Template 4.0
    void read_product_definition_template0_() {
      read_product_definition_template0_or_template8_();
      reader_.skip(section_size_ - 34);
    }

    // Template 4.8
    void read_product_definition_template8_() {
      read_product_definition_template0_or_template8_();
      reader_.read<uint16_t>("Year");
      reader_.read<uint8_t>("Month");
      reader_.read<uint8_t>("Day");
      reader_.read<uint8_t>("Hour");
      reader_.read<uint8_t>("Minute");
      reader_.read<uint8_t>("Second");
      reader_.read<uint8_t>("n - Number of time range specifications describing the time intervals used to calculate the statistically processed field");
      reader_.read<uint32_t>("Total number of data values missing in statistical process");
      reader_.skip(section_size_ - 46);
    }

    // Section 5
    void read_data_representation_section_() {
      reader_.read<uint32_t>("Number of data points where one or more values are specified in Section 7 when a bit map is present, total number of data points when a bit map is absent");
      data_representation_template_ = reader_.read<uint16_t>("Data Representation Template Number");
      switch (data_representation_template_) {
        case 0:
          read_data_representation_template0_();
          break;
        default:
          if (trace()) {
            *trace() << "!!! IGNORE UNKNOWN TEMPLATE !!!\n";
          }
          reader_.skip(section_size_ - 12);
      }
    }

    // Template 5.0
    void read_data_representation_template0_() {
      ctx_.r = reader_.read<float>("Reference value");
      ctx_.e = reader_.read_1s_complement<int16_t>("Binary scale factor");
      ctx_.d = reader_.read_1s_complement<int16_t>("Decimal scale factor");

      int bits = reader_.read<uint8_t>("Number of bits used for each packed value for simple packing, or for each group reference value for complex packing or spatial differencing");
      BOOST_ASSERT(bits == 12);

      reader_.read<uint8_t>("Type of original field values");

      reader_.skip(section_size_ - 21);
    }

    // Section 6
    void read_bitmap_section_() {
      reader_.read<uint8_t>("Bit-map indicator");
      reader_.skip(section_size_ - 6);
    }

    // Section 7
    void read_data_section_() {
      switch (data_representation_template_) {
        case 0:
          read_data_data_representation_template0_();
          break;
        default:
          if (trace()) {
            *trace() << "!!! IGNORE UNKNOWN TEMPLATE !!!\n";
          }
          reader_.skip(section_size_ - 5);
      }
    }

    // Template 7.0
    void read_data_data_representation_template0_() {
      std::vector<uint8_t> buffer(section_size_ - 5);
      reader_.read_buffer(buffer);

      std::vector<double> data(ctx_.n_i * ctx_.n_j);
      for (uint32_t j = 0; j < ctx_.n_j; ++j) {
        for (uint32_t i = 0; i < ctx_.n_i; ++i) {
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
  };
}

#endif
