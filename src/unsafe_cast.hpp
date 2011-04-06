#ifndef SIMPATICO_UNSAFE_CAST_HPP
#define SIMPATICO_UNSAFE_CAST_HPP

#include <stddef.h>

namespace simpatico {
  template <typename T>
  class unsafe_cast {
  public:
    explicit unsafe_cast() {
      data_.value = T();
    }

    T const& get() const {
      return data_.value;
    }

    char* data() {
      return data_.buffer;
    }

    char const* data() const {
      return data_.buffer;
    }

    size_t size() const {
      return sizeof(T);
    }

    char* begin() {
      return data_.buffer;
    }

    char const* begin() const {
      return data_.buffer;
    }

    char* end() {
      return data_.buffer + sizeof(T);
    }

    char const* end() const {
      return data_.buffer + sizeof(T);
    }

  private:
    union {
      T value;
      char buffer[sizeof(T)];
    } data_;
  };
}

#endif
