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
#include "msm_context.hpp"
#include "msm_reader.hpp"
#include "pregrid_writer.hpp"

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
  simpatico::pregrid_writer writer_;

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
      simpatico::msm_context const& msm_context,
      std::vector<double> const& data) {
    if (msm_context.forecast_time > 0) {
      return;
    }

    std::string hdate;
    {
      std::ostringstream out;
      boost::posix_time::time_facet* hdate_facet
        = new boost::posix_time::time_facet();
      out.imbue(std::locale(out.getloc(), hdate_facet));
      hdate_facet->format("%Y-%m-%d_%H:%M:%S");
      out << boost::posix_time::ptime_from_tm(msm_context.reference_time);
      hdate = out.str();
    }

    std::string field;
    std::string units;
    std::string desc;
    switch (msm_context.parameter_category) {
      case 0:
        switch (msm_context.parameter_number) {
          case 0: // Temperature [K]
            field = "T";
            units = "K";
            desc = "T [K]";
            break;
        }
        break;
      case 1:
        switch (msm_context.parameter_number) {
          case 1: // Relative humidity [%]
            field = "RH";
            units = "%";
            desc = "RH [%]";
            break;
          case 8: // Total precipitation [kg/m^2]
            break;
        }
        break;
      case 2:
        switch (msm_context.parameter_number) {
          case 2: // u-component of wind [m/s]
            field = "U";
            units = "m s{-1}";
            desc = "U";
            break;
          case 3: // v-component of wind [m/s]
            field = "V";
            units = "m s{-1}";
            desc = "V";
            break;
          case 8: // Vertical velocity (pressure) [Pa/s]
            break;
        }
        break;
      case 3:
        switch (msm_context.parameter_number) {
          case 0: // Pressure [Pa]
            field = "PRESSURE";
            units = "Pa";
            desc = "PRESSURE [Pa]";
            break;
          case 1: // Pressure reduced to MSL [Pa]
            field = "PMSL";
            units = "Pa";
            desc = "PMSL [Pa]";
            break;
          case 5: // Geopotential height [gpm]
            field = "HGT";
            units = "m";
            desc = "HGT [m]";
            break;
        }
        break;
      case 6:
        switch (msm_context.parameter_number) {
          case 1: // Total cloud cover [%]
            break;
          case 3: // Low cloud cover [%]
            break;
          case 4: // Medium cloud cover [%]
            break;
          case 5: // High cloud cover [%]
            break;
        }
        break;
    }

    if (field.empty()) {
      return;
    }

    float xlvl = simpatico::pregrid_context::surface();
    if (msm_context.surface_type == 100) {
      xlvl = msm_context.surface_value;
    }

    if (trace0_) {
      *trace0_
          << "hdate: " << hdate << "\n"
          << "field: " << field << "\n"
          << "units: " << units << "\n"
          << "desc: " << desc << "\n"
          << "xlvl: " << xlvl << "\n";
    }

    simpatico::pregrid_context pregrid_context = {
      3,                // ifv
      hdate,
      0,                // xcfst
      field,
      units,
      desc,
      xlvl,
      msm_context.n_i,  // nx
      msm_context.n_j,  // ny
      0,                // iproj
      msm_context.la_1, // startlat
      msm_context.lo_1, // startlon
      -msm_context.d_j, // deltalat
      msm_context.d_i,  // deltalon
    };
    writer_.write_context(pregrid_context);

    size_t size = msm_context.n_i * msm_context.n_j;
    writer_.record_start();
    for (size_t i = 0; i < size; ++i) {
      writer_.write<float>(data[i]);
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
