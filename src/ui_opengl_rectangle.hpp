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
        is_grid_point_(image.is_grid_point()),
        data_(size_.x * size_.y * 4),
        is_first_(true) {
      double const min = image.data_min();
      double const max = image.data_max();

      for (int y = 0; y < size_.y; ++y) {
        for (int x = 0; x < size_.x; ++x) {
          vm::Color4b color(f(image.data(vm::Point2i(x, y)), min, max));
          size_t i = (x + y * size_.x) * 4;
          data_[i]     = color.x;
          data_[i + 1] = color.y;
          data_[i + 2] = color.z;
          data_[i + 3] = color.w;
        }
      }
    }

    void draw_opengl() {
      glEnable(GL_TEXTURE);
      glEnable(GL_TEXTURE_RECTANGLE_EXT);

      if (is_first_) {
        is_first_ = false;
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexParameteri(
            GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(
            GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(
            GL_TEXTURE_RECTANGLE_EXT, 0,
            GL_RGBA, size_.x, size_.y, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, &data_[0]);
      }

      vm::TexCoord2d t0;
      vm::TexCoord2d t1(size_.x, size_.y);
      if (is_grid_point_) {
        t0.add(vm::Vector2d(0.5, 0.5));
        t1.sub(vm::Vector2d(0.5, 0.5));
      }

      glBegin(GL_QUADS);
      glColor3d(1, 1, 1);
      glTexCoord2d(t0.x, t0.y);
      glVertex2d(start_.x, start_.y);
      glTexCoord2d(t0.x, t1.y);
      glVertex2d(start_.x, ended_.y);
      glTexCoord2d(t1.x, t1.y);
      glVertex2d(ended_.x, ended_.y);
      glTexCoord2d(t1.x, t0.y);
      glVertex2d(ended_.x, start_.y);
      glEnd();

      glDisable(GL_TEXTURE);
      glDisable(GL_TEXTURE_RECTANGLE_EXT);
    }

  public:
    vm::Point2d start_;
    vm::Point2d ended_;
    vm::Tuple2i size_;
    bool is_grid_point_;
    std::vector<uint8_t> data_;
    bool is_first_;
  };
}

#endif
