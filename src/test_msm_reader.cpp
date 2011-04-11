#include <fstream>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "msm_reader.hpp"

namespace {
  inline void read_cb(
      simpatico::msm_reader::context const& ctx,
      std::vector<double> const& data) {}

  BOOST_AUTO_TEST_CASE(test_msm_reader0) {
    std::ifstream in(
        "../data/Z__C_RJTD_20110314000000_MSM_GPV_Rjp_Lsurf_FH00-15_grib2.bin",
        std::ios::in | std::ios::binary);
    simpatico::msm_reader reader(in, read_cb);
    reader.read();
  }

  BOOST_AUTO_TEST_CASE(test_msm_reader1) {
    std::ifstream in(
        "../data/Z__C_RJTD_20110314000000_MSM_GPV_Rjp_L-pall_FH00-15_grib2.bin",
        std::ios::in | std::ios::binary);
    simpatico::msm_reader reader(in, read_cb);
    reader.read();
  }
}
