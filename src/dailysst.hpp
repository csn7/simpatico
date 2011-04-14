#ifndef SIMPATICO_DAILYSST_HPP
#define SIMPATICO_DAILYSST_HPP

#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/swap.hpp>
#include "dailysst_context.hpp"
#include "dailysst_reader.hpp"
#include "image.hpp"

namespace simpatico {
  namespace dailysst {
    inline void read_images_cb(
        std::vector<boost::shared_ptr<image> >& images,
        dailysst_context const& context,
        std::vector<int> const& data) {
      boost::posix_time::ptime time
        = boost::posix_time::ptime_from_tm(context.time);

      std::vector<double> image_data;
      BOOST_FOREACH(int i, data) {
        switch (i) {
          case 777:
            image_data.push_back(-10);
            break;
          case 888:
            image_data.push_back(-20);
            break;
          case 999:
            image_data.push_back(-30);
            break;
          default:
            image_data.push_back(i * 0.1);
        }
      }

      boost::shared_ptr<simpatico::image> image
        = boost::make_shared<simpatico::image>(
            "DAILYSST",
            context.start,
            context.ended,
            context.size,
            image_data,
            false);

      image->meta_add("time",  boost::posix_time::to_iso_extended_string(time));
      image->meta_add("start", boost::lexical_cast<std::string>(context.start));
      image->meta_add("ended", boost::lexical_cast<std::string>(context.ended));
      image->meta_add("size",  boost::lexical_cast<std::string>(context.size));

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
      dailysst_reader reader(
          in, boost::bind(read_images_cb, boost::ref(source), _1, _2));
      reader.read();
      boost::swap(source, target);
      return true;
    }
  }
}

#endif
