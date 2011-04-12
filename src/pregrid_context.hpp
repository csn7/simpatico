#ifndef SIMPATICO_PREGRID_CONTEXT_HPP
#define SIMPATICO_PREGRID_CONTEXT_HPP

#include <stdint.h>
#include <string>

namespace simpatico {
  struct pregrid_context {
    int32_t     ifv;
    std::string hdate;
    float       xcfst;
    std::string field;
    std::string units;
    std::string desc;
    float       xlvl;
    int32_t     nx;
    int32_t     ny;
    int32_t     iproj;
    float       startlat;
    float       startlon;
    float       deltalat;
    float       deltalon;
    float       dx;
    float       dy;
    float       xlonc;
    float       truelat1;
    float       truelat2;
  };
}

#endif
