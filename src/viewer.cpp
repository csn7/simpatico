#include <stdlib.h>
#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/gl.h>
#include <FL/gl_draw.H>
#include "assert.hpp"
#include "chooser.hpp"
#include "dailysst.hpp"
#include "msm.hpp"
#include "pregrid.hpp"
#include "ui_color.hpp"
#include "ui_opengl_camera2d.hpp"
#include "ui_opengl_rectangle.hpp"
#include "ui_opengl_window.hpp"
#include "ui_table.hpp"

#include "viewer_ui.hxx"

class application : boost::noncopyable {
public:
  static void idle(void* self) {
    static_cast<application*>(self)->idle_();
  }

  application()
    : image_color_(
          boost::bind(
              simpatico::ui_color::linear,
              vm::Color4d(0, 0, 1, 1),
              vm::Color4d(1, 0, 0, 1),
              _1, _2, _3)) {}

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
    image_meta->function(
        boost::bind(
            &application::draw_image_meta_cell,
            this, _1, _2, _3, _4, _5, _6, _7));
    camera2d_.function(
        boost::bind(
            &application::notify_camera2d,
            this));
    return Fl::run();
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

  void draw_image_meta_cell(
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

  void notify_camera2d() {
    camera2d_radius->value(camera2d_.radius());
    camera2d_center_x->value(camera2d_.center().x);
    camera2d_center_y->value(camera2d_.center().y);
  }

  void open_msm() {
    try {
      boost::optional<std::string> path = simpatico::chooser::browse_file();
      if (! path) {
        return;
      }
      if (! simpatico::msm::read_images(*path, images_)) {
        return;
      }
      initialize_image_(*path);
    } catch (std::exception const& e) {
      fl_alert("%s", e.what());
    }
  }

  void open_dailysst() {
    try {
      boost::optional<std::string> path = simpatico::chooser::browse_file();
      if (! path) {
        return;
      }
      if (! simpatico::dailysst::read_images(*path, images_)) {
        return;
      }
      initialize_image_(*path);
    } catch (std::exception const& e) {
      fl_alert("%s", e.what());
    }
  }

  void open_pregrid() {
    try {
      boost::optional<std::string> path = simpatico::chooser::browse_file();
      if (! path) {
        return;
      }
      if (! simpatico::pregrid::read_images(*path, images_)) {
        return;
      }
      initialize_image_(*path);
    } catch (std::exception const& e) {
      fl_alert("%s", e.what());
    }
  }

  void save_screenshot() {
    boost::optional<std::string> path = simpatico::chooser::browse_save_file();
    if (! path) {
      return;
    }
    opengl_window->save_screenshot(*path);
  }

  void update_camera2d() {
    camera2d_.radius(camera2d_radius->value());
    camera2d_.center(
        vm::Point3d(
            camera2d_center_x->value(),
            camera2d_center_y->value(),
            camera2d_.center().z));
    camera2d_.eye(
        vm::Point3d(
            camera2d_center_x->value(),
            camera2d_center_y->value(),
            camera2d_.eye().z));
    opengl_window->redraw();
  }

  void reset_camera2d() {
    if (image_) {
      camera2d_.reset(image_->start(), image_->ended());
      opengl_window->redraw();
    }
  }

  void select_image_name() {
    image_ = images_[image_name->value()];
    rectangle_ = boost::make_shared<simpatico::ui_opengl_rectangle>(
        *image_, image_color_);

    image_meta->col_header(1);
    image_meta->col_resize(1);
    image_meta->cols(2);
    image_meta->rows(image_->meta_size());

    camera2d_.reset(image_->start(), image_->ended());

    {
      std::ostringstream out;
      out << "min: " << image_->data_min() << ", "
          << "max: " << image_->data_max();
      text_ = out.str();
    }

    opengl_window->redraw();
  }

  template <typename T_function>
  void select_image_color(T_function image_color) {
    image_color_ = image_color;
    if (image_) {
      rectangle_ = boost::make_shared<simpatico::ui_opengl_rectangle>(
          *image_, image_color_);
    }
    opengl_window->redraw();
  }

  void quit() {
    exit(0);
  }

private:
  std::vector<boost::shared_ptr<simpatico::image> > images_;
  boost::shared_ptr<simpatico::image> image_;
  boost::function<vm::Color4b (double, double, double)> image_color_;

  simpatico::ui_opengl_camera2d camera2d_;
  boost::shared_ptr<simpatico::ui_opengl_rectangle> rectangle_;
  boost::optional<std::string> text_;

  void idle_() {
  }

  void initialize_image_(const std::string& path) {
    using boost::xpressive::s1;
    using boost::xpressive::set;

    static boost::xpressive::sregex const regex((s1= (set= '&', '/', '\\', '_')));
    static std::string const format = "\\\\$1";

    image_path->value(path.c_str());
    image_name->clear();
    BOOST_FOREACH(boost::shared_ptr<simpatico::image> i, images_) {
      image_name->add(
          boost::xpressive::regex_replace(
              i->name(),
              regex,
              format,
              boost::xpressive::regex_constants::format_perl).c_str(), 0, 0);
    }
    image_meta->clear();

    // update ui
    image_name->value(0);
    select_image_name();
  }
};

application app;

#include "viewer_ui.cxx"

int main(int argc, char* argv[]) {
  Fl::add_idle(application::idle, &app);
  make_window();
  return app.run(argc, argv);
}
