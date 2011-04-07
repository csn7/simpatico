#ifndef SIMPATICO_UI_OPENGL_WINDOW_HPP
#define SIMPATICO_UI_OPENGL_WINDOW_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <boost/function.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>

#include <png.h>
#ifndef int_p_NULL
#define int_p_NULL (int*)0
#endif
#include <boost/gil/image.hpp>
#include <boost/gil/extension/io/png_io.hpp>

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

    void save_screenshot(std::string const& path) const {
      std::vector<uint8_t> data(w() * h() * 4);
      glReadPixels(0, 0, w(), h(), GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

      boost::gil::rgba8_image_t image(w(), h());
      boost::gil::rgba8_image_t::view_t::iterator out
        = boost::gil::view(image).begin();
      for (int y = h() - 1; y >= 0; --y) {
        for (int x = 0; x < w(); ++x) {
          int i = (x + y * w()) * 4;
          *out = boost::gil::rgba8_pixel_t(
              data[i], data[i + 1], data[i + 2], data[i + 3]);
          ++out;
        }
      }
      boost::gil::png_write_view(path.c_str(), boost::gil::const_view(image));
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
