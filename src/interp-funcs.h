
#ifndef INTERP_FUNCS_GET_YOU_WHERE_YOU_GO
#define INTERP_FUNCS_GET_YOU_WHERE_YOU_GO


//#include "slappy-types.h"
#include "base_types.hpp"


namespace zeugma  {


namespace InterpFuncs
{ using INTERP_FUNC = f64 (*)(f64);
  extern f64 ASYMP_B (f64 t);
  extern f64 ASYMP_A (f64 t);
  extern INTERP_FUNC ASYMP; // = ASYMP_B;
  extern f64 ASYMP_AB (f64 t);
  extern f64 LINEAR (f64 t);
  extern f64 QUADRATIC_B (f64 t);
  extern f64 QUADRATIC_A (f64 t);
  extern INTERP_FUNC QUADRATIC; // = QUADRATIC_B;
  extern f64 QUADRATIC_AB (f64 t);
  extern f64 CUBIC_B (f64 t);
  extern f64 CUBIC_A (f64 t);
  extern INTERP_FUNC CUBIC; // = CUBIC_B
  extern f64 CUBIC_AB (f64 t);
}


}  // it's the big sleep for namespace zeugma


#endif
