#ifndef SIMPATICO_IMAGE_HPP
#define SIMPATICO_IMAGE_HPP

#include <algorithm>
#include <limits>
#include <stddef.h>
#include <string>
#include <utility>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include "trim.hpp"
#include "vecmath.hpp"

namespace simpatico {
  class image : boost::noncopyable {
  public:
    enum grid_type {
      grid_point,
      grid_area,
    };

    template <typename T>
    explicit image(
        std::string const& name,
        vm::Point2d const& start,
        vm::Point2d const& ended,
        vm::Tuple2i const& size,
        std::vector<T> const& data,
        bool is_grid_point = true)
      : name_(name),
        start_(start),
        ended_(ended),
        size_(size),
        is_grid_point_(is_grid_point),
        data_min_(+std::numeric_limits<double>::max()),
        data_max_(-std::numeric_limits<double>::max()) {
      data_.reserve(data.size());
      BOOST_FOREACH(double i, data) {
        data_.push_back(i);
        data_min_ = std::min(i, data_min_);
        data_max_ = std::max(i, data_max_);
      }
    }

    std::string const& name() const {
      return name_;
    }

    vm::Point2d const& start() const {
      return start_;
    }

    vm::Point2d const& ended() const {
      return ended_;
    }

    vm::Tuple2i const& size() const {
      return size_;
    }

    bool is_grid_point() const {
      return is_grid_point_;
    }

    double data(vm::Point2i const& point) const {
      return data_[point.x + point.y * size_.x];
    }

    double data_min() const {
      return data_min_;
    }

    double data_max() const {
      return data_max_;
    }

    template <typename T>
    void meta_add(std::string const& key, T const& value) {
      meta_.push_back(
          std::make_pair(
              key,
              trim(boost::lexical_cast<std::string>(value))));
    }

    size_t meta_size() const {
      return meta_.size();
    }

    std::string const& meta_key(size_t i) {
      return meta_[i].first;
    }

    std::string const& meta_value(size_t i) {
      return meta_[i].second;
    }

  private:
    std::string name_;
    vm::Point2d start_;
    vm::Point2d ended_;
    vm::Tuple2i size_;
    bool is_grid_point_;
    std::vector<double> data_;
    double data_min_;
    double data_max_;
    std::vector<std::pair<std::string, std::string> > meta_;
  };
}

#endif
