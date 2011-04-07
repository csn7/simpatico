#ifndef SIMPATICO_UI_OPENGL_WINDOW_HPP
#define SIMPATICO_UI_OPENGL_WINDOW_HPP

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <boost/function.hpp>

namespace simpatico {
  class ui_opengl_window : public Fl_Gl_Window {
  public:
    typedef boost::function<int (int)> handle_function_type;
    typedef boost::function<void ()> draw_function_type;

    explicit ui_opengl_window(int x, int y, int w, int h, char const* label = 0)
      : Fl_Gl_Window(x, y, w, h, label) {}

    void handle_function(handle_function_type const& handle_function) {
      handle_function_ = handle_function;
    }

    handle_function_type const& handle_function() const {
      return handle_function_;
    }

    void draw_function(draw_function_type const& draw_function) {
      draw_function_ = draw_function;
    }

    draw_function_type const& draw_function() const {
      return draw_function_;
    }

    virtual int handle(int event) {
      if (handle_function_) {
        int const result = handle_function_(event);
        if (result != 0) {
          return result;
        }
      }
      return Fl_Gl_Window::handle(event);
    }

    virtual void draw() {
      if (draw_function_) {
        draw_function_();
      }
    }

  private:
    handle_function_type handle_function_;
    draw_function_type draw_function_;
  };
}

#endif
