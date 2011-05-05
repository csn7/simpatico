#ifndef SIMPATICO_PREGRID_HPP
#define SIMPATICO_PREGRID_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/swap.hpp>
#include "image.hpp"
#include "pregrid_context.hpp"
#include "pregrid_reader.hpp"
#include "trim.hpp"
#include "vecmath.hpp"

namespace simpatico {
  namespace pregrid {
    inline void read_images_cb(
        std::vector<boost::shared_ptr<image> >& images,
        pregrid_context const& context,
        std::vector<float> const& data) {
      std::ostringstream name;
      name << trim(context.hdate)
          << " " << context.xlvl
          << " " << trim(context.desc);

      vm::Point2d start;;
      vm::Point2d ended;
      if (context.iproj == 0) {
        // [LNG], [LAT]
        start.set(context.startlon, context.startlat);
        ended.set(
            context.deltalon * (context.nx - 1),
            context.deltalat * (context.ny - 1));
        ended.add(start);
      } else {
        // [km]
        ended.set(context.dx * (context.nx - 1), context.dy * (context.ny - 1));
      }
      boost::shared_ptr<simpatico::image> image
        = boost::make_shared<simpatico::image>(
            name.str(), start, ended, vm::Tuple2i(context.nx, context.ny), data);

      image->meta_add("ifv",      context.ifv);
      image->meta_add("hdate",    context.hdate);
      image->meta_add("xcfst",    context.xcfst);
      image->meta_add("field",    context.field);
      image->meta_add("units",    context.units);
      image->meta_add("desc",     context.desc);
      image->meta_add("xlvl",     context.xlvl);
      image->meta_add("nx",       context.nx);
      image->meta_add("ny",       context.ny);
      image->meta_add("iproj",    context.iproj);
      image->meta_add("startlat", context.startlat);
      image->meta_add("startlon", context.startlon);
      image->meta_add("deltalat", context.deltalat);
      image->meta_add("deltalat", context.deltalon);
      image->meta_add("dx",       context.dx);
      image->meta_add("dy",       context.dy);
      image->meta_add("xlonc",    context.xlonc);
      image->meta_add("truelat1", context.truelat1);
      image->meta_add("truelat2", context.truelat2);

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
      pregrid_reader reader(
          in, boost::bind(read_images_cb, boost::ref(source), _1, _2), &std::cout);
      reader.read();
      boost::swap(source, target);
      return true;
    }
  }
}

#endif
