#ifndef SIMPATICO_DAILYSST_CONTEXT_HPP
#define SIMPATICO_DAILYSST_CONTEXT_HPP

#include <time.h>
#include "vecmath.hpp"

namespace simpatico {
  struct dailysst_context {
    struct tm time;
    vm::Point2d start;
    vm::Point2d ended;
    vm::Tuple2i size;
  };
}

#endif
