#include <math.h>
#include <iostream>
#include <limits>
#include <boost/test/unit_test.hpp>
#include "geodetic_system.hpp"

namespace {
  BOOST_AUTO_TEST_CASE(test_geodetic_system_latitude_distance) {
    // http://ja.wikipedia.org/wiki/%E7%B7%AF%E5%BA%A6

    static const double LATITUDE[] = { // [deg]
      0, 35, 90,
    };

    static const double DISTANCE[] = { // [m]
      30.7, 30.8, 31.0,
    };

    simpatico::geodetic_system grs80
      = simpatico::geodetic_system::make_grs80();

    for (int i = 0; i < 3; ++i) {
      double distance = grs80.latitude_distance(LATITUDE[i], 1 / 3600.0); // [m];
      BOOST_CHECK(abs(distance - DISTANCE[i]) < 0.05);
    }
  }

  BOOST_AUTO_TEST_CASE(test_geodetic_system_longitude_distance) {
    // http://ja.wikipedia.org/wiki/%E7%B5%8C%E5%BA%A6

    static const double DISTANCE[] = { // [km]
      111.319, 107.550, 96.486, 78.847, 55.800, 28.902, 0,
    };

    simpatico::geodetic_system grs80
      = simpatico::geodetic_system::make_grs80();

    for (int i = 0; i < 7; ++i) {
      double distance = grs80.longtitude_distance(i * 15, 1) * 0.001; // [km]
      BOOST_CHECK(abs(distance - DISTANCE[i]) < 0.001);
    }
  }

  BOOST_AUTO_TEST_CASE(test_geodetic_system_1f) {
    simpatico::geodetic_system grs80
      = simpatico::geodetic_system::make_grs80();

    // 福島第一原子力発電所付近のMSM気圧面の格子幅 [km]
    std::cout
        << "u: " << grs80.longtitude_distance(37.422, 0.125) * 0.001 << "\n"
        << "v: " << grs80.latitude_distance(37.422, 0.1) * 0.001 << "\n";
  }
}
