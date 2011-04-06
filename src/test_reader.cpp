#include <stdint.h>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "reader.hpp"

namespace {
  BOOST_AUTO_TEST_CASE(test_reader0) {
    std::istringstream in("foo\xFE\xED\xFA\xCE\xFF\xFF\x80\x01");
    simpatico::reader<> reader(in);
    BOOST_CHECK(reader.read_string(3) == "foo");
    BOOST_CHECK(reader.read<uint32_t>() == 0xFEEDFACE);
    BOOST_CHECK(reader.read<int16_t>() == -1);
    BOOST_CHECK(reader.read_1s_complement<int16_t>() == -1);
  }
}
