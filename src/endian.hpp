#ifndef SIMPATICO_ENDIAN_HPP
#define SIMPATICO_ENDIAN_HPP

#include <algorithm>
#include <boost/detail/endian.hpp>

namespace simpatico {
  template <
    bool T_data_is_big_endian,
    bool T_host_is_big_endian = BOOST_BYTE_ORDER == 4321
  >
  struct endian {
    template <typename T>
    static void swap(T begin, T end) {
      std::reverse(begin, end);
    }
  };

  template <bool T_is_big_endian>
  struct endian<T_is_big_endian, T_is_big_endian> {
    template <typename T>
    static void swap(T begin, T end) {}
  };
}

#endif
