#include <fstream>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "pregrid_reader.hpp"

namespace {
  inline void read_cb(
      simpatico::pregrid_reader::context const& ctx,
      std::vector<float> const& data) {
  }

  BOOST_AUTO_TEST_CASE(test_pregrid_reader) {
    std::ifstream in("../data/ON84:1993-03-13_00", std::ios::in | std::ios::binary);
    simpatico::pregrid_reader reader(in, read_cb);
    reader.read();
  }
}
