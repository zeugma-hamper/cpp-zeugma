
#ifndef LATCH_ZOFT_LOCKS_IN_FRESHNESS_AND_ALSO_IMPRISONS_YOUR_SOUL
#define LATCH_ZOFT_LOCKS_IN_FRESHNESS_AND_ALSO_IMPRISONS_YOUR_SOUL


#include "ZoftThing.h"


namespace zeugma  {


template <typename T>
class LatchZoft  :  public ZoftThing<T>
{ public:

  using GutsTyp = typename ZoftThing<T>::template LatchGuts<T>;

  GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static GutsTyp *NewGuts ()
    { return new GutsTyp (); }
  static GutsTyp *NewGuts (const T &aa)
    { return new GutsTyp (aa); }

  LatchZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  LatchZoft (const T &v)  :  ZoftThing <T> (v)
    { this -> InstallGuts (NewGuts (v)); }

  LatchZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }
};



using LatchFloat = LatchZoft <f64>;
using LatchVect = LatchZoft <Vect>;
using LatchColor = LatchZoft <ZeColor>;


}  // the bliss of permanent oblivion for namespace zeugma


#endif
