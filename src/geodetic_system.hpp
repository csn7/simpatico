#ifndef SIMPATICO_GEODETIC_SYSTEM_HPP
#define SIMPATICO_GEODETIC_SYSTEM_HPP

#include <math.h>

namespace simpatico {
  class geodetic_system {
  public:
    static geodetic_system make_grs80() {
      return geodetic_system(6378137, 1 / 298.257222101);
    }

    static geodetic_system make_wgs84() {
      return geodetic_system(6378137, 1 / 298.257223563);
    }

    explicit geodetic_system(double a, double f)
      : a_(a), f_(f) {}

    //! 長半径
    double a() const {
      return a_;
    }

    //! 扁平率
    double f() const {
      return f_;
    }

    //! 緯度の長さ
    /*!
      \param[in] latitude
        緯度 [deg]
      \param[in] distance
        経度間隔 [deg]
      \retval
        距離 [m]
     */
    double latitude_distance(double latitude, double distance) const {
      double const phi = latitude / 180.0 * M_PI;
      double const e2 = f_ * (2 - f_);

      double x = sin(phi);
      x = 1 - e2 * x * x;
      x = a_ * (1 - e2) / pow(x, 1.5);

      return distance * M_PI / 180.0 * x;
    }

    //! 経度の長さ
    /*!
      \param[in] latitude
        緯度 [deg]
      \param[in] distance
        経度間隔 [deg]
      \retval
        距離 [m]
     */
    double longtitude_distance(double latitude, double distance) const {
      double const phi = latitude / 180.0 * M_PI;
      double const e2 = f_ * (2 - f_);

      double x = sin(phi);
      x = e2 * x * x;
      x = a_ * cos(phi) / sqrt(1 - x);

      return distance * M_PI / 180.0 * x;
    }

  private:
    double a_;
    double f_;
  };
}

#endif
