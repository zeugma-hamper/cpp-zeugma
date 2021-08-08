
//
// (c) treadle & loam, provisioners llc
//

#include "interp-funcs.h"

#include <math.h>


namespace zeugma  {


namespace InterpFuncs
{
#define JANUSIZE(LFT_F,RGT_F)                  \
  ((t < 0.5)  ?  (0.5 * LFT_F (2.0 * t))       \
   :  (0.5 + 0.5 * RGT_F (2.0 * t - 1.0)))

f64 ASYMP_B (f64 t)  { return 1.0 - exp (-7.62462 * t); }
f64 ASYMP_A (f64 t)  { return exp (-7.62462 * (1.0 - t)); }
INTERP_FUNC ASYMP = ASYMP_B;
f64 ASYMP_AB (f64 t)  { return JANUSIZE (ASYMP_A, ASYMP_B); }
f64 LINEAR (f64 t)  { return t; }
f64 QUADRATIC_B (f64 t)  { return t * (2.0 - t); }
f64 QUADRATIC_A (f64 t)  { return t * t; }
INTERP_FUNC QUADRATIC = QUADRATIC_B;
f64 QUADRATIC_AB (f64 t)  { return JANUSIZE (QUADRATIC_A, QUADRATIC_B); }
f64 CUBIC_B (f64 t)  { return t * (3.0 - t * (3.0 - t)); }
f64 CUBIC_A (f64 t)  { return t * t * t; }
INTERP_FUNC CUBIC = CUBIC_B;
f64 CUBIC_AB (f64 t)  { return JANUSIZE (CUBIC_A, CUBIC_B); }

#undef JANUSIZE
}


}  // pushing up the daisies, namespace zeugma is.
