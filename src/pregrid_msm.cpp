#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/noncopyable.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/xpressive/xpressive.hpp>
#include "assert.hpp"
#include "fortran_writer.hpp"
#include "msm_context.hpp"
#include "msm_reader.hpp"
#include "pad.hpp"

class application : boost::noncopyable {
public:
  explicit application(
      std::string const& surface_msm_file,
      std::string const& isobaric_surface_msm_file,
      std::string const& output_file,
      std::ostream* trace0 = 0,
      std::ostream* trace1 = 0)
    : surface_msm_file_(surface_msm_file),
      isobaric_surface_msm_file_(isobaric_surface_msm_file),
      output_file_(output_file),
      trace0_(trace0),
      trace1_(trace1),
      writer_(out_) {}

  void run() {
    if (trace0_) {
      *trace0_
          << "surface_msm_file: "
          << surface_msm_file_
          << "\n"
          << "isobaric_surface_msm_file: "
          << isobaric_surface_msm_file_
          << "\n"
          << "output_file: "
          << output_file_
          << "\n";
    }

    out_.open(output_file_.c_str(), std::ios::out | std::ios::binary);
    BOOST_ASSERT(out_);

    read_msm_(surface_msm_file_);
    read_msm_(isobaric_surface_msm_file_);
  }

private:
  std::string surface_msm_file_;
  std::string isobaric_surface_msm_file_;
  std::string output_file_;
  std::ostream* trace0_;
  std::ostream* trace1_;

  std::ofstream out_;
  simpatico::fortran_writer<> writer_;

  void read_msm_(std::string const& path) {
    using boost::xpressive::eos;
    using boost::xpressive::icase;

    static boost::xpressive::sregex const regex(icase(".bz2") >> eos);

    boost::iostreams::filtering_istream in;
    if (boost::xpressive::regex_search(path, regex)) {
      in.push(boost::iostreams::bzip2_decompressor());
    }
    in.push(boost::iostreams::file_source(path, std::ios::in | std::ios::binary));

    simpatico::msm_reader reader(
        in, boost::bind(&application::read_msm_cb_, this, _1, _2), trace1_);
    reader.read();
  }

  void read_msm_cb_(
      simpatico::msm_context const& context,
      std::vector<double> const& data) {

    if (context.forecast_time > 0) {
      return;
    }

    boost::posix_time::ptime reference_time
      = boost::posix_time::ptime_from_tm(context.reference_time);

    std::string hdate;
    {
      std::ostringstream out;
      boost::posix_time::time_facet* hdate_facet
        = new boost::posix_time::time_facet();
      out.imbue(std::locale(out.getloc(), hdate_facet));
      hdate_facet->format("%Y-%m-%d_%H:%M:%S");
      out << reference_time;
      hdate = simpatico::pad(out.str(), 24);
    }

    if (trace0_) {
      *trace0_ << "hdate: " << hdate << "\n";
    }
  }

  void write_pregrid_() {
    writer_.record_start();
    {
      writer_.write<int32_t>(3);
    }
    writer_.record_ended();
  }
};

int main(int argc, char* argv[]) {
  namespace po = boost::program_options;

  try {
    po::options_description options_description("Options");
    options_description.add_options()
      ("help,h",                                              "Output this help")
      ("surface-msm-file",          po::value<std::string>(), "Surface MSM file")
      ("isobaric-surface-msm-file", po::value<std::string>(), "Isobaric surface MSM file")
      ("output-file,o",             po::value<std::string>(), "Output file")
      ("verbose,v",                 po::value<int>(),         "Verbose level");

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

    int verbose = 0;
    if (variables_map.count("verbose") > 0) {
      verbose = variables_map["verbose"].as<int>();
    }

    application app(
        variables_map["surface-msm-file"].as<std::string>(),
        variables_map["isobaric-surface-msm-file"].as<std::string>(),
        variables_map["output-file"].as<std::string>(),
        verbose > 0 ? &std::cout : 0,
        verbose > 1 ? &std::cout : 0);
    app.run();

    return 0;
  } catch (std::exception const& e) {
    std::cerr << e.what() << "\n";
  }
  return 1;
}
