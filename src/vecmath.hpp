#ifndef SIMPATICO_VECMATH_HPP
#define SIMPATICO_VECMATH_HPP

#include <iostream>
#include <vecmath.h>

namespace kh_vecmath {
  template <typename T>
  inline std::ostream& operator<<(std::ostream& out, Tuple2<T> const& t) {
    return out << "(" << t.x << "," << t.y << ")";
  }

  template <typename T>
  inline std::ostream& operator<<(std::ostream& out, Tuple3<T> const& t) {
    return out << "(" << t.x << "," << t.y << "," << t.z << ")";
  }

  template <typename T>
  inline std::ostream& operator<<(std::ostream& out, Tuple4<T> const& t) {
    return out << "(" << t.x << "," << t.y << "," << t.z << "," << t.w << ")";
  }

  typedef Tuple2<int> Tuple2i;
  typedef Point2<int> Point2i;
}

namespace vm = kh_vecmath;

#endif
