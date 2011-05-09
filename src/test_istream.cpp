#include <stddef.h>
#include <iostream>
#include <sstream>
#include <string>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/test/unit_test.hpp>

namespace {
  void test(std::istream& in) {
    std::vector<char> buffer(256);
    BOOST_CHECK(boost::iostreams::read(in, &buffer[0], 256) == 256);
    BOOST_CHECK(boost::iostreams::read(in, &buffer[0], 256) == 244);
  }

  BOOST_AUTO_TEST_CASE(test_std_istringstream) {
    std::string buffer;
    for (size_t i = 0; i < 500; ++i) {
      buffer += "X";
    }
    std::istringstream in(buffer);
    test(in);
  }

  BOOST_AUTO_TEST_CASE(test_bzip2) {
    std::string buffer;

    std::ostringstream sout;
    {
      boost::iostreams::filtering_ostream fout;
      fout.push(boost::iostreams::bzip2_compressor());
      fout.push(sout);
      for (size_t i = 0; i < 500; ++i) {
        fout << "X";
      }
    }
    buffer = sout.str();

    std::istringstream sin(buffer);

    boost::iostreams::filtering_istream fin;
    fin.push(boost::iostreams::bzip2_decompressor());
    fin.push(sin);
    test(fin);
  }
}
