#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>

namespace {





}

int main(int argc, char* argv[]) {
  namespace po = boost::program_options;

  try {
    po::options_description options_description("Options");
    options_description.add_options()
      ("help,h",                                              "Output this help")
      ("surface-msm-file",          po::value<std::string>(), "Surface MSM file")
      ("isobaric-surface-msm-file", po::value<std::string>(), "Isobaric surface MSM file")
      ("output-file,o",             po::value<std::string>(), "Output file")
      ("verbose,v",                                           "Verbose mode");

    po::positional_options_description positional_options_description;
    positional_options_description.add("surface-msm-file",          1);
    positional_options_description.add("isobaric-surface-msm-file", 1);
    positional_options_description.add("output-file",               1);

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
          << " SURFACE_MSM_FILE ISOBARIC_SURFACE_MSM_FILE -o OUTPUT_FILE\n"
          << "\n"
          << options_description;
      return 0;
    }

    bool verbose = variables_map.count("verbose") > 0;
    if (verbose) {
      std::cout
          << variables_map["surface-msm-file"].as<std::string>()
          << "\n"
          << variables_map["isobaric-surface-msm-file"].as<std::string>()
          << "\n"
          << variables_map["output-file"].as<std::string>()
          << "\n";
    }

    return 0;
  } catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  return 1;
}
