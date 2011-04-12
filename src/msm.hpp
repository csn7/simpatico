#ifndef SIMPATICO_MSM_HPP
#define SIMPATICO_MSM_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/swap.hpp>
#include "image.hpp"
#include "msm_context.hpp"
#include "msm_reader.hpp"

namespace simpatico {
  namespace msm {
    inline std::string parameter(int parameter_category, int parameter_number) {
      switch (parameter_category) {
        case 0:
          switch (parameter_number) {
            case 0: return "Temperature [K]";
          }
          break;
        case 1:
          switch (parameter_number) {
            case 1: return "Relative humidity [%]";
            case 8: return "Total precipitation [kg/m^2]";
          }
          break;
        case 2:
          switch (parameter_number) {
            case 2: return "u-component of wind [m/s]";
            case 3: return "v-component of wind [m/s]";
            case 8: return "Vertical velocity (pressure) [Pa/s]";
          }
          break;
        case 3:
          switch (parameter_number) {
            case 0: return "Pressure [Pa]";
            case 1: return "Pressure reduced to MSL [Pa]";
            case 5: return "Geopotential height [gpm]";
          }
          break;
        case 6:
          switch (parameter_number) {
            case 1: return "Total cloud cover [%]";
            case 3: return "Low cloud cover [%]";
            case 4: return "Medium cloud cover [%]";
            case 5: return "High cloud cover [%]";
          }
          break;
      }
      return std::string();
    }

    inline void read_images_cb(
        std::vector<boost::shared_ptr<image> >& images,
        msm_context const& context,
        std::vector<double> const& data) {

      boost::posix_time::ptime reference_time
        = boost::posix_time::ptime_from_tm(context.reference_time);
      reference_time += boost::posix_time::seconds(context.forecast_time);

      std::ostringstream name;
      name << boost::posix_time::to_iso_extended_string(reference_time);
      if (context.surface_type == 100) {
        name << " " << context.surface_value;
      }
      name << " " << parameter(context.parameter_category, context.parameter_number);

      boost::shared_ptr<simpatico::image> image
        = boost::make_shared<simpatico::image>(
            name.str(),
            vm::Point2d(context.lo_1, context.la_1),
            vm::Point2d(context.lo_2, context.la_2),
            vm::Tuple2i(context.n_i, context.n_j),
            data);

      image->meta_add(
          "reference_time",
          boost::posix_time::to_iso_extended_string(reference_time));
      image->meta_add("n_i", context.n_i);
      image->meta_add("n_j", context.n_j);
      image->meta_add("la_1", context.la_1);
      image->meta_add("lo_1", context.lo_1);
      image->meta_add("la_2", context.la_2);
      image->meta_add("lo_2", context.lo_2);
      image->meta_add("d_i", context.d_i);
      image->meta_add("d_j", context.d_j);
      image->meta_add("parameter_category", context.parameter_category);
      image->meta_add("parameter_number", context.parameter_number);
      image->meta_add("forecast_time", context.forecast_time);
      image->meta_add("surface_type", context.surface_type);
      image->meta_add("surface_value", context.surface_value);
      image->meta_add("r", context.r);
      image->meta_add("e", context.e);
      image->meta_add("d", context.d);

      images.push_back(image);
    }

    inline bool read_images(
        std::string const& path,
        std::vector<boost::shared_ptr<image> >& target) {
      std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
      if (! in) {
        return false;
      }

      std::vector<boost::shared_ptr<image> > source;
      msm_reader reader(
          in, boost::bind(read_images_cb, boost::ref(source), _1, _2));
      reader.read();
      boost::swap(source, target);
      return true;
    }
  }
}

#endif
