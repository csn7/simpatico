#ifndef SIMPATICO_MSM_CONTEXT_HPP
#define SIMPATICO_MSM_CONTEXT_HPP

#include <time.h>

namespace simpatico {
  struct msm_context {
    struct tm reference_time;
    uint32_t n_i;
    uint32_t n_j;
    double la_1;
    double lo_1;
    double la_2;
    double lo_2;
    double d_i;
    double d_j;
    int parameter_category;
    int parameter_number;
    double forecast_time;
    int surface_type;
    double surface_value;
    double r;
    int e;
    int d;
  };
}

#endif
