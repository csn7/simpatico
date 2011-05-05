#ifndef SIMPATICO_UI_OPENGL_CAMERA2D_HPP
#define SIMPATICO_UI_OPENGL_CAMERA2D_HPP

#include <math.h>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include "vecmath.hpp"

namespace simpatico {
  class ui_opengl_camera2d : boost::noncopyable {
  public:
    typedef boost::function<void ()> function_type;

    explicit ui_opengl_camera2d()
      : radius_(1), center_(), eye_(0, 0, 1), up_(0, 1, 0) {}

    void function(function_type const& function) {
      function_ = function;
    }

    function_type const& function() const {
      return function_;
    }

    void radius(double radius) {
      radius_ = radius;
    }

    double radius() const {
      return radius_;
    }

    void center(vm::Point3d const& center) {
      center_ = center;
    }

    vm::Point3d const& center() const {
      return center_;
    }

    void eye(vm::Point3d const& eye) {
      eye_ = eye;
    }

    vm::Point3d const& eye() const {
      return eye_;
    }

    vm::Vector3d const& up() const {
      return up_;
    }

    void reset(vm::Point2d const& start, vm::Point2d const& ended) {
      vm::Point2d center(start);
      center.add(ended);
      center.scale(0.5);

      radius_ = start.distance(ended) * 0.5;
      center_.set(center.x, center.y, 0);
      eye_.set(center.x, center.y, 1);
      notify_();
    }

    int handle_opengl(int event) {
      switch (event) {
        case FL_PUSH:
          if (Fl::event_button() == FL_LEFT_MOUSE) {
            mouse_push_();
            return 1;
          }
          break;
        case FL_DRAG:
          if (Fl::event_button() == FL_LEFT_MOUSE) {
            mouse_drag_();
            notify_();
            return 1;
          }
          break;
        case FL_MOUSEWHEEL:
          radius_ *= pow(0.99, -Fl::event_dy());
          notify_();
          return 1;
      }
      return 0;
    }

    void draw_opengl(int w, int h) {
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
    function_type function_;
    double radius_;
    vm::Point3d center_;
    vm::Point3d eye_;
    vm::Vector3d up_;
    boost::array<int, 4> viewport_;
    vm::Matrix4d projection_;
    vm::Point3d mouse_start_;
    vm::Point3d mouse_eye_;

    vm::Point3d transform_(vm::Point2i const& source) {
      vm::Vector2d half_size(viewport_[2], viewport_[3]);
      half_size.scale(0.5);

      vm::Vector2d center(viewport_[0], viewport_[1]);
      center.add(half_size);

      vm::Point3d target;
      projection_.transform(
          vm::Point3d(
              - (source.x - center.x) / half_size.x,
              + (source.y - center.y) / half_size.y,
              0),
          &target);
      return target;
    }

    void notify_() {
      if (function_) {
        function_();
      }
    }

    void mouse_push_() {
      glGetIntegerv(GL_VIEWPORT, viewport_.c_array());

      glGetDoublev(GL_PROJECTION_MATRIX, &projection_.m00);
      projection_.transpose();
      projection_.invert();

      mouse_start_.set(transform_(vm::Point2i(Fl::event_x(), Fl::event_y())));
      mouse_eye_.set(eye_);
    }

    void mouse_drag_() {
      vm::Vector3d move(transform_(vm::Point2i(Fl::event_x(), Fl::event_y())));
      move.sub(mouse_start_);
      eye_.set(mouse_eye_);
      eye_.add(move);
      center_.set(eye_.x, eye_.y, 0);
    }
  };
}

#endif
