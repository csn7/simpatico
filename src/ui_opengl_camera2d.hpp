#ifndef SIMPATICO_UI_OPENGL_CAMERA2D_HPP
#define SIMPATICO_UI_OPENGL_CAMERA2D_HPP

#include <math.h>
#include <algorithm>
#include <boost/noncopyable.hpp>
#include <FL/Fl.H>
#include <FL/gl.H>
#include "vecmath.hpp"

namespace simpatico {
  class ui_opengl_camera2d : boost::noncopyable {
  public:
    explicit ui_opengl_camera2d()
      : radius_(1),
        center_(),
        eye_(0, 0, 1),
        up_(0, 1, 0) {}

    void reset(vm::Point2d const& start, vm::Point2d const& ended) {
      vm::Point2d center(start);
      center.add(ended);
      center.scale(0.5);

      radius_ = start.distance(ended) * 0.5;
      center_.set(center.x, center.y, 0);
      eye_.set(center.x, center.y, 1);
    }

    int handle_opengl(int event) {
      switch (event) {
        case FL_MOUSEWHEEL:
          radius_ *= pow(0.99, -Fl::event_dy());
          return 1;
      }
      return 0;
    }

    void draw_opengl(int w, int h) const {
      double const ratio = w / double(h);

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-radius_* ratio, radius_ * ratio, -radius_, radius_, -1, 1);
      gluLookAt(
          eye_.x,    eye_.y,    eye_.z,
          center_.x, center_.y, center_.z,
          up_.x,     up_.y,     up_.z);
    }

  private:
    double radius_;
    vm::Point3d center_;
    vm::Point3d eye_;
    vm::Vector3d up_;
  };
}

#endif
