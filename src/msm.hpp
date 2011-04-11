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
#include "msm_reader.hpp"

namespace simpatico {
  namespace msm {
    inline void read_cb(
        std::vector<boost::shared_ptr<image> >& images,
        msm_reader::context const& ctx,
        std::vector<double> const& data) {
      boost::posix_time::ptime reference_time
        = boost::posix_time::ptime_from_tm(ctx.reference_time);

      std::ostringstream name;
      name << reference_time;
      if (ctx.surface_type == 100) {
        name << " " << ctx.surface_value;
      }
      name
          << "[" << ctx.parameter_category
          << "," << ctx.parameter_number << "]";

      boost::shared_ptr<simpatico::image> image
        = boost::make_shared<simpatico::image>(
            name.str(),
            vm::Point2d(ctx.lo_1, ctx.la_1),
            vm::Point2d(ctx.lo_2, ctx.la_2),
            vm::Tuple2i(ctx.n_i, ctx.n_j),
            data);

      image->meta_add("reference_time", reference_time);
      image->meta_add("n_i", ctx.n_i);
      image->meta_add("n_j", ctx.n_j);
      image->meta_add("la_1", ctx.la_1);
      image->meta_add("lo_1", ctx.lo_1);
      image->meta_add("la_2", ctx.la_2);
      image->meta_add("lo_2", ctx.lo_2);
      image->meta_add("d_i", ctx.d_i);
      image->meta_add("d_j", ctx.d_j);
      image->meta_add("parameter_category", ctx.parameter_category);
      image->meta_add("parameter_number", ctx.parameter_number);
      image->meta_add("surface_type", ctx.surface_type);
      image->meta_add("surface_value", ctx.surface_value);
      image->meta_add("r", ctx.r);
      image->meta_add("e", ctx.e);
      image->meta_add("d", ctx.d);

      images.push_back(image);
    }

    inline bool read(
        std::string const& path,
        std::vector<boost::shared_ptr<image> >& target) {
      std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
      if (! in) {
        return false;
      }

      std::vector<boost::shared_ptr<image> > source;
      msm_reader reader(in, boost::bind(read_cb, boost::ref(source), _1, _2));
      reader.read();
      boost::swap(source, target);
      return true;
    }
  }
}

#endif
