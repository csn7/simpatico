#include <expat.h>
#include <vector>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/test/unit_test.hpp>
#include "reader.hpp"

namespace {
  inline void XMLCALL start(
      void* data, char const* element, char const** attribute) {

    std::cout << element << "\n";
  }

  inline void XMLCALL ended(
      void* data, char const* element) {
  }

  BOOST_AUTO_TEST_CASE(test_expat) {
    static char const* path = "../test_data/L03/L03-b-06_6740-jgd.xml.bz2";

    boost::shared_ptr<boost::remove_pointer<XML_Parser>::type> parser(
        XML_ParserCreate(0),
        XML_ParserFree);
    XML_SetElementHandler(parser.get(), start, ended);

    boost::iostreams::filtering_istream in;
    in.push(boost::iostreams::bzip2_decompressor());
    in.push(boost::iostreams::file_source(path, std::ios::in | std::ios::binary));
    simpatico::reader<false> reader(in);

    std::vector<char> buffer(4096);

    size_t position_previous = 0;
    bool done;
    do {
      reader.read_buffer(buffer);

      size_t position = reader.position();
      size_t size = position - position_previous;
      position_previous = position;

      done = size < buffer.size();
      if (XML_Parse(parser.get(), &buffer[0], size, done) == XML_STATUS_ERROR) {
        std::cerr
            << "XML_Parse error: "
            << XML_ErrorString(XML_GetErrorCode(parser.get()))
            << " at file "
            << path
            << " line "
            << XML_GetCurrentLineNumber(parser.get())
            << "\n";
        BOOST_ERROR("XML_Parse error");
      }
    } while (! done);
  }
}
