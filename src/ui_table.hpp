#ifndef SIMPATICO_UI_TABLE_HPP
#define SIMPATICO_UI_TABLE_HPP

#include <boost/function.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Table.H>

namespace simpatico {
  class ui_table : public Fl_Table {
  public:
    typedef boost::function<
      void (Fl_Table::TableContext, int, int, int, int, int, int)
    > function_type;

    explicit ui_table(int x, int y, int w, int h, char const* label = 0)
      : Fl_Table(x, y, w, h, label) {}

    void function(function_type const& function) {
      function_ = function;
    }

    function_type const& function() const {
      return function_;
    }

    virtual void draw_cell(
        Fl_Table::TableContext ctx, int r, int c, int x, int y, int w, int h) {
      if (function_) {
        function_(ctx, r, c, x, y, w, h);
      }
    }

  private:
    function_type function_;
  };
}

#endif
