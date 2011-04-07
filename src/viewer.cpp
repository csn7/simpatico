#include <stdlib.h>
#include <string>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/gl.H>
#include "chooser.hpp"
#include "opengl_window.hpp"
#include "pregrid.hpp"
#include "ui_table.hpp"

#include "viewer_ui.hxx"

class application : boost::noncopyable {
public:
  static void idle(void* self) {
    static_cast<application*>(self)->idle_();
  }

  int run(int argc, char* argv[]) {
    window->show(argc, argv);
    opengl_window->function(
        boost::bind(
            &application::draw_opengl,
            this));
    pregrid_meta->function(
        boost::bind(
            &application::draw_pregrid_cell,
            this, _1, _2, _3, _4, _5, _6, _7));
    return Fl::run();
  }

  void draw_opengl() {
    int const w = opengl_window->w();
    int const h = opengl_window->h();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void draw_pregrid_cell(
      Fl_Table::TableContext ctx, int r, int c, int x, int y, int w, int h) {
    static int const padding = 2;
    static char const* const header[] = { "Name", "Value" };

    if (ctx == Fl_Table::CONTEXT_COL_HEADER) {
      fl_push_clip(x, y, w, h);
      fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, FL_BACKGROUND_COLOR);
      fl_color(FL_FOREGROUND_COLOR);
      fl_draw(header[c], x + padding, y, w - padding, h, FL_ALIGN_LEFT);
      fl_pop_clip();
    } else if (ctx == Fl_Table::CONTEXT_CELL) {
      fl_push_clip(x, y, w, h);
      fl_draw_box(FL_THIN_UP_BOX, x, y, w, h, FL_BACKGROUND2_COLOR);
      fl_color(FL_FOREGROUND_COLOR);
      fl_draw(
          c == 0
            ? image_->meta_key(r).c_str()
            : image_->meta_value(r).c_str(),
          x + padding, y, w - padding, h, FL_ALIGN_LEFT);
      fl_pop_clip();
    }
  }

  void open_pregrid() {
    boost::optional<std::string> path = simpatico::chooser::browse_file();
    if (! path) {
      return;
    }

    if (! simpatico::pregrid::read(*path, pregrid_)) {
      return;
    }

    pregrid_path->value(path->c_str());
    pregrid_image->clear();
    BOOST_FOREACH(boost::shared_ptr<simpatico::image> i, pregrid_) {
      pregrid_image->add(i->name().c_str());
    }
    pregrid_meta->clear();
  }

  void select_pregrid_image() {
    image_ = pregrid_[pregrid_image->value()];

    pregrid_meta->col_header(1);
    pregrid_meta->col_resize(1);
    pregrid_meta->cols(2);
    pregrid_meta->rows(image_->meta_size());
  }

  void quit() {
    exit(0);
  }

private:
  boost::shared_ptr<simpatico::image> image_;
  std::vector<boost::shared_ptr<simpatico::image> > pregrid_;

  void idle_() {
  }
};

application app;

#include "viewer_ui.cxx"

int main(int argc, char* argv[]) {
  Fl::add_idle(application::idle, &app);
  make_window();
  return app.run(argc, argv);
}
