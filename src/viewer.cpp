#include <stdlib.h>
#include <string>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/ref.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/gl.H>
#include <FL/glu.H>
#include "chooser.hpp"
#include "pregrid.hpp"
#include "ui_opengl_camera2d.hpp"
#include "ui_opengl_rectangle.hpp"
#include "ui_opengl_window.hpp"
#include "ui_table.hpp"

#include "viewer_ui.hxx"

vm::Color4b color(double value, double min, double max) {
  double alpha = (value - min) / (max - min);
  return vm::Color4b(alpha * 255, 0, (1 - alpha) * 255, 255);
}

class application : boost::noncopyable {
public:
  static void idle(void* self) {
    static_cast<application*>(self)->idle_();
  }

  int run(int argc, char* argv[]) {
    window->show(argc, argv);
    opengl_window->draw_function(
        boost::bind(
            &application::draw_opengl,
            this));
    opengl_window->handle_function(
        boost::bind(
            &application::handle_opengl,
            this, _1));
    pregrid_meta->function(
        boost::bind(
            &application::draw_pregrid_cell,
            this, _1, _2, _3, _4, _5, _6, _7));
    return Fl::run();
  }

  int handle_opengl(int event) {
    {
      int const result = camera2d_.handle_opengl(event);
      if (result != 0) {
        opengl_window->redraw();
        return result;
      }
    }
    return 0;
  }

  void draw_opengl() {
    int const w = opengl_window->w();
    int const h = opengl_window->h();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera2d_.draw_opengl(w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (rectangle_) {
      rectangle_->draw_opengl();
    }

    if (text_) {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      glOrtho(0, w, h, 0, -1, 1);
      glColor3d(1, 1, 1);
      gl_font(FL_HELVETICA, 16);
      gl_draw(text_->c_str(), 10, 20);
      glPopMatrix();
    }

    glFinish();
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
    rectangle_ = boost::make_shared<simpatico::ui_opengl_rectangle>(*image_, color);

    pregrid_meta->col_header(1);
    pregrid_meta->col_resize(1);
    pregrid_meta->cols(2);
    pregrid_meta->rows(image_->meta_size());

    camera2d_.reset(image_->start(), image_->ended());

    {
      std::ostringstream out;
      out << "min: " << image_->data_min() << ", "
          << "max: " << image_->data_max();
      text_ = out.str();
    }

    opengl_window->redraw();
  }

  void quit() {
    exit(0);
  }

private:
  std::vector<boost::shared_ptr<simpatico::image> > pregrid_;
  boost::shared_ptr<simpatico::image> image_;

  simpatico::ui_opengl_camera2d camera2d_;

  boost::optional<std::string> text_;
  boost::shared_ptr<simpatico::ui_opengl_rectangle> rectangle_;

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
