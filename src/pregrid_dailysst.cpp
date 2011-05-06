#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include "dailysst_context.hpp"
#include "dailysst_reader.hpp"
#include "fortran_writer.hpp"

class application : boost::noncopyable {
public:
  explicit application(
      std::string const& dailysst_file,
      std::string const& output_file,
      std::ostream* trace = 0)
    : dailysst_file_(dailysst_file),
      output_file_(output_file),
      trace_(trace),
      writer_(out_) {}

  void run() {
    if (trace_) {
      *trace_
          << "dailysst_file: "
          << dailysst_file_
          << "\n"
          << "output_file: "
          << output_file_
          << "\n";
    }

    out_.open(output_file_.c_str(), std::ios::out | std::ios::binary);
    BOOST_ASSERT(out_);

    std::ifstream in(dailysst_file_.c_str(), std::ios::in | std::ios::binary);
    BOOST_ASSERT(in);
    simpatico::dailysst_reader reader(
        in, boost::bind(&application::read_dailysst_cb_, this, _1, _2));
    reader.read();
  }

private:
  std::string dailysst_file_;
  std::string output_file_;
  std::ostream* trace_;

  std::ofstream out_;
  simpatico::fortran_writer<> writer_;

  void read_dailysst_cb_(
      simpatico::dailysst_context const& context,
      std::vector<int> const& data) {
  }
};

int main(int argc, char* argv[]) {
  namespace po = boost::program_options;

  try {
    po::options_description options_description("Options");
    options_description.add_options()
      ("help,h",                                  "Output this help")
      ("dailysst-file", po::value<std::string>(), "DAILYSST file")
      ("output-file,o", po::value<std::string>(), "Output file")
      ("verbose,v",                               "Verbose level");

    po::positional_options_description positional_options_description;
    positional_options_description.add("dailysst-file", 1);
    positional_options_description.add("output-file",   1);

    po::variables_map variables_map;
    po::store(
        po::command_line_parser(argc, argv)
          .options(options_description)
          .positional(positional_options_description)
          .run(), variables_map);
    po::notify(variables_map);

    if (variables_map.count("help")) {
      std::cout
          << "Usage:\n"
          << "  "
          << argv[0]
          << " DAILYSST_FILE -o OUTPUT_FILE\n"
          << "\n"
          << options_description;
      return 0;
    }

    application app(
        variables_map["dailysst-file"].as<std::string>(),
        variables_map["output-file"].as<std::string>(),
        variables_map.count("verbose") > 0 ? &std::cout : 0);
    app.run();

    return 0;
  } catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  return 1;
}
