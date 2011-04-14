#ifndef SIMPATICO_DAILYSST_READER_HPP
#define SIMPATICO_DAILYSST_READER_HPP

#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include "dailysst_context.hpp"
#include "trim.hpp"

namespace simpatico {
  // http://near-goos1.jodc.go.jp/rdmdb/format/JMA/dailysst.txt
  class dailysst_reader : boost::noncopyable {
  public:
    typedef boost::function<
      void (dailysst_context const&, std::vector<int> const& data)
    > function_type;

    explicit dailysst_reader(std::istream& in, function_type const& function)
      : in_(in), function_(function) {
      context_.start.set(120, 50);
      context_.ended.set(160, 20);
      context_.size.set(160, 120);
    }

    void read() {
      read_header_();
      read_();
    }

  private:
    std::istream& in_;
    function_type function_;
    dailysst_context context_;

    void read_header_() {
      std::string header;
      std::getline(in_, header);
      std::istringstream in(header);

      memset(&context_.time, 0, sizeof(context_.time));
      in >> context_.time.tm_year;
      context_.time.tm_year -= 1900;
      in >> context_.time.tm_mon;
      --context_.time.tm_mon;
      in >> context_.time.tm_mday;
    }

    void read_() {
      std::vector<int> data;
      for (int j = 0; j < context_.size.y; ++j) {
        std::string buffer;
        std::getline(in_, buffer);
        for (int i = 0; i < context_.size.x; ++i) {
          data.push_back(boost::lexical_cast<int>(trim(buffer.substr(i * 3, 3))));
        }
      }
      function_(context_, data);
    }
  };
}

#endif
