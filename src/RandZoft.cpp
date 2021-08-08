
//
// (c) treadle & loam, provisioners llc
//

#include "RandZoft.h"


namespace zeugma  {


template <> f64 UnifMultiRand <f64> ()
  { return drand48 (); }

template <> Vect UnifMultiRand <Vect> ()
  { return Vect (drand48 (), drand48 (), drand48 ()); }

template <> ZeColor UnifMultiRand <ZeColor> ()
  { return ZeColor (drand48 (), drand48 (), drand48 (), drand48 ()); }


}
