#ifndef SIMPATICO_OPENGL_WINDOW_HPP
#define SIMPATICO_OPENGL_WINDOW_HPP

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <boost/function.hpp>

namespace simpatico {
  class opengl_window : public Fl_Gl_Window {
  public:
    typedef boost::function<void ()> function_type;

    explicit opengl_window(int x, int y, int w, int h, char const* label = 0)
      : Fl_Gl_Window(x, y, w, h, label) {}

    void function(function_type function) {
      function_ = function;
    }

    function_type const& function() const {
      return function_;
    }

    virtual void draw() {
      if (function_) {
        function_();
      }
    }

  private:
    function_type function_;
  };
}

#endif
