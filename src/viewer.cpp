#include <stdlib.h> // exit
#include <boost/noncopyable.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

namespace simpatico {
  class application : boost::noncopyable {
  public:
    static void idle(void* self) {
      static_cast<application*>(self)->idle_();
    }

    explicit application()
      : window_() {}

    void window(Fl_Window* window) {
      window_ = window;
    }

    Fl_Window* window() const {
      return window_;
    }

    void quit() {
      exit(0);
    }

  private:
    Fl_Window* window_;

    void idle_() {
    }
  };
}

simpatico::application app;

#include "viewer_ui.cxx"

int main(int argc, char* argv[]) {
  Fl::add_idle(simpatico::application::idle, &app);
  app.window(make_window());
  app.window()->show(argc, argv);
  return Fl::run();
}
