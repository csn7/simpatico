#ifndef SIMPATICO_CHOOSER_HPP
#define SIMPATICO_CHOOSER_HPP

#include <string>
#include <boost/optional.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>

namespace simpatico {
  namespace chooser {
    inline boost::optional<std::string> browse_file(char const* title = 0) {
      Fl_Native_File_Chooser chooser;
      chooser.title(title);
      chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
      if (chooser.show() == 0) {
        return std::string(chooser.filename());
      } else {
        return boost::optional<std::string>();
      }
    }
  }
}

#endif
