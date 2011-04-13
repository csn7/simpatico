#include <fstream>
#include <vector>
#include <boost/test/unit_test.hpp>
#include "msm_context.hpp"
#include "msm_reader.hpp"

namespace {
  inline void read_cb(
      simpatico::msm_context const& context,
      std::vector<double> const& data) {}

  BOOST_AUTO_TEST_CASE(test_msm_reader_surface) {
    std::ifstream in(
        "../test_data/MSM_GPV/Z__C_RJTD_20110314060000_MSM_GPV_Rjp_L-pall_FH00-15_grib2.bin",
        std::ios::in | std::ios::binary);
    simpatico::msm_reader reader(in, read_cb);
    reader.read();
  }

  BOOST_AUTO_TEST_CASE(test_msm_reader_isobaric_surface) {
    std::ifstream in(
        "../test_data/MSM_GPV/Z__C_RJTD_20110314060000_MSM_GPV_Rjp_L-pall_FH00-15_grib2.bin",
        std::ios::in | std::ios::binary);
    simpatico::msm_reader reader(in, read_cb);
    reader.read();
  }
}
