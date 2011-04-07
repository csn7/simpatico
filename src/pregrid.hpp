#ifndef SIMPATICO_PREGRID_HPP
#define SIMPATICO_PREGRID_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include "image.hpp"
#include "pregrid_reader.hpp"
#include "trim.hpp"
#include "vecmath.hpp"

namespace simpatico {
  namespace pregrid {
    inline void read_cb(
        std::vector<boost::shared_ptr<image> >& images,
        pregrid_reader::context const& ctx,
        std::vector<float> const& data) {
      std::ostringstream name;
      name << trim(ctx.hdate) << " " << ctx.xlvl << " " << trim(ctx.desc);

      vm::Point2d start;;
      vm::Point2d ended;
      if (ctx.iproj == 0) {
        // [LNG], [LAT]
        start.set(ctx.startlon, ctx.startlat);
        ended.set(ctx.deltalon * (ctx.nx -1), ctx.deltalat * (ctx.ny - 1));
        ended.add(start);
      } else {
        // [km]
        ended.set(ctx.dx * (ctx.nx - 1), ctx.dy * (ctx.ny - 1));
      }
      boost::shared_ptr<simpatico::image> image
        = boost::make_shared<simpatico::image>(
            name.str(), start, ended, vm::Tuple2i(ctx.nx, ctx.ny), data);

      image->meta_add("ifv",      ctx.ifv);
      image->meta_add("hdate",    ctx.hdate);
      image->meta_add("xcfst",    ctx.xcfst);
      image->meta_add("field",    ctx.field);
      image->meta_add("units",    ctx.units);
      image->meta_add("desc",     ctx.desc);
      image->meta_add("xlvl",     ctx.xlvl);
      image->meta_add("nx",       ctx.nx);
      image->meta_add("ny",       ctx.ny);
      image->meta_add("iproj",    ctx.iproj);
      image->meta_add("startlat", ctx.startlat);
      image->meta_add("startlon", ctx.startlon);
      image->meta_add("deltalat", ctx.deltalat);
      image->meta_add("deltalat", ctx.deltalon);
      image->meta_add("dx",       ctx.dx);
      image->meta_add("dy",       ctx.dy);
      image->meta_add("xlonc",    ctx.xlonc);
      image->meta_add("truelat1", ctx.truelat1);
      image->meta_add("truelat2", ctx.truelat2);

      images.push_back(image);
    }

    inline bool read(
        std::string const& path,
        std::vector<boost::shared_ptr<image> >& images) {
      std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
      if (! in) {
        return false;
      }

      images.clear();
      pregrid_reader reader(
          in,
          boost::bind(read_cb, boost::ref(images), _1, _2),
          &std::cout);
      reader.read();
      return true;
    }
  }
}

#endif
