#ifndef SIMPATICO_OPEN_IFSTREAM_HPP
#define SIMPATICO_OPEN_IFSTREAM_HPP

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/counter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/xpressive/xpressive.hpp>

namespace simpatico {
  namespace iostreams {
    using boost::iostreams::filtering_istream;

    inline boost::shared_ptr<filtering_istream> open(char const* path) {
      using boost::xpressive::icase;
      using boost::xpressive::eos;

      boost::shared_ptr<filtering_istream> in
        = boost::make_shared<filtering_istream>();

      

    }

  }


  boost::shared_ptr<std::istream> open_ifstream(char const* path) {
    using boost::xpressive::icase;
    using boost::xpressive::eos;

    static boost::xpressive::sregex const regex_bzip2(icase(".bz2") >> eos);
    static boost::xpressive::sregex const regex_gzip(icase(".gz") >> eos);

    boost::shared_ptr<boost::iostreams::filtering_istream> in
      = boost::make_shared<boost::iostreams::filtering_istream>();
    if (boost::xpressive::regex_search(path, regex)) {
      in->push(boost::iostreams::bzip2_decompressor());
    }
    in->push(boost::iostreams::file_source(path, std::ios::in | std::ios::binary));

    return in;
  }
}

#endif
