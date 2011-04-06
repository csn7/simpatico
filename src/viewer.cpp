#include <stdlib.h>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/gl.H>
#include "opengl_window.hpp"

class application : boost::noncopyable {
public:
  static void idle(void* self) {
    static_cast<application*>(self)->idle_();
  }

  explicit application()
    : window_(), opengl_window_() {}

  void window(Fl_Window* window) {
    window_ = window;
  }

  void opengl_window(simpatico::opengl_window* opengl_window) {
    opengl_window_ = opengl_window;
    opengl_window_->function(boost::bind(&application::draw, this));
  }

  void show(int argc, char* argv[]) {
    BOOST_ASSERT(window_);
    window_->show(argc, argv);
  }

  void draw() {
    BOOST_ASSERT(opengl_window_);

    int const w = opengl_window_->w();
    int const h = opengl_window_->h();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void quit() {
    exit(0);
  }

private:
  Fl_Window* window_;
  simpatico::opengl_window* opengl_window_;

  void idle_() {
  }
};

application app;

#include "viewer_ui.cxx"

int main(int argc, char* argv[]) {
  Fl::add_idle(application::idle, &app);
  app.window(make_window());
  app.opengl_window(opengl_window);
  app.show(argc, argv);
  return Fl::run();
}
