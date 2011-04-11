#ifndef SIMPATICO_UI_COLOR_HPP
#define SIMPATICO_UI_COLOR_HPP

#include "vecmath.hpp"

namespace simpatico {
  namespace ui_color {
    inline vm::Color4b linear(
        vm::Color4d const& color0,
        vm::Color4d const& color1,
        double value, double min, double max) {
      vm::Color4d color;
      color.interpolate(color0, color1, (value - min) / (max - min));
      return vm::Color4b(
          color.x * 255, color.y * 255, color.z * 255, color.w * 255);
    }
  }
}

#endif
