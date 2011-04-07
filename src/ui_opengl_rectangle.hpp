#ifndef SIMPATICO_UI_OPENGL_RECTANGLE_HPP
#define SIMPATICO_UI_OPENGL_RECTANGLE_HPP

#include <stdint.h>
#include <algorithm>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <FL/Fl.H>
#include <FL/gl.H>
#include "image.hpp"
#include "vecmath.hpp"

namespace simpatico {
  class ui_opengl_rectangle : boost::noncopyable {
  public:
    template <typename T_function>
    explicit ui_opengl_rectangle(simpatico::image const& image, T_function f)
      : start_(image.start()),
        ended_(image.ended()),
        size_(image.size()),
        data_(size_.x * size_.y * 4) {
      double const min = image.data_min();
      double const max = image.data_max();
      if (min == max) {
        std::fill(data_.begin(), data_.end(), 0xFF);
      } else {
        for (int y = 0; y < size_.y; ++y) {
          for (int x = 0; x < size_.x; ++x) {
            vm::Color4b color = f(image.data(vm::Point2i(x, y)), min, max);
            size_t i = (x + y * size_.x) * 4;
            data_[i]     = color.x;
            data_[i + 1] = color.y;
            data_[i + 2] = color.z;
            data_[i + 3] = color.w;
          }
        }
      }
    }

    void draw_opengl() const {
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_RECTANGLE_EXT);

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
      glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexImage2D(
          GL_TEXTURE_RECTANGLE_EXT, 0,
          GL_RGBA, size_.x, size_.y, 0,
          GL_RGBA, GL_UNSIGNED_BYTE, &data_[0]);

      glBegin(GL_QUADS);
      glColor3d(1, 1, 1);
      glTexCoord2d(0, 0);
      glVertex2d(start_.x, start_.y);
      glTexCoord2d(0, size_.y);
      glVertex2d(start_.x, ended_.y);
      glTexCoord2d(size_.x, size_.y);
      glVertex2d(ended_.x, ended_.y);
      glTexCoord2d(size_.x, 0);
      glVertex2d(ended_.x, start_.y);
      glEnd();

      glDisable(GL_TEXTURE_2D);
      glDisable(GL_TEXTURE_RECTANGLE_EXT);
    }

  public:
    vm::Point2d start_;
    vm::Point2d ended_;
    vm::Tuple2i size_;
    std::vector<uint8_t> data_;
  };
}

#endif
