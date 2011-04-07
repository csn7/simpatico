#ifndef SIMPATICO_CHOOSER_HPP
#define SIMPATICO_CHOOSER_HPP

#include <string>
#include <boost/optional.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>

namespace simpatico {
  namespace chooser {
    namespace detail {
      inline boost::optional<std::string> browse_file(int type, char const* title) {
        Fl_Native_File_Chooser chooser;
        chooser.type(type);
        chooser.title(title);
        if (chooser.show() == 0) {
          return std::string(chooser.filename());
        } else {
          return boost::optional<std::string>();
        }
      }
    }

    inline boost::optional<std::string> browse_file(char const* title = 0) {
      return detail::browse_file(Fl_Native_File_Chooser::BROWSE_FILE, title);
    }

    inline boost::optional<std::string> browse_save_file(char const* title = 0) {
      return detail::browse_file(Fl_Native_File_Chooser::BROWSE_SAVE_FILE, title);
    }
  }
}

#endif
