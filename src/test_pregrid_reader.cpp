#include <fstream>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "pregrid_context.hpp"
#include "pregrid_reader.hpp"

namespace {
  inline void read_cb(
      simpatico::pregrid_context const& context,
      std::vector<float> const& data) {
  }

  BOOST_AUTO_TEST_CASE(test_pregrid_reader) {
    std::ifstream in(
        "../test_data/pregrid/ON84:1993-03-13_00",
        std::ios::in | std::ios::binary);
    simpatico::pregrid_reader reader(in, read_cb);
    reader.read();
  }
}
