
#ifndef RAND_ZOFT_KNOWS_EXACTLY_WHERE_IT_WILL_BE
#define RAND_ZOFT_KNOWS_EXACTLY_WHERE_IT_WILL_BE


#include "ZoftThing.h"

#include <stdlib.h>


namespace zeugma  {


template <typename T> T UnifMultiRand ();

template <> f64 UnifMultiRand <f64> ();
template <> Vect UnifMultiRand <Vect> ();
template <> ZeColor UnifMultiRand <ZeColor> ();

// specializations actually, ahem, defined over in .cpp file...


template <typename T>
class RandZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        ZoftThing <TT> ampl;
        ZoftThing <TT> cent;

        ZGuts ()  :  ZoftThing<TT>::template ZGuts<TT> (),
           ampl (TT (1.0)), cent (TT (0.0))
          { }
        ZGuts (const TT &a)  :  ZoftThing<TT>::template ZGuts<TT> (),
           ampl (a), cent (TT (0.0))
          { }
        ZGuts (const TT &a, const TT &c)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              ampl (a), cent (c)
          { }
        ZGuts (const ZoftThing <TT> &az, const ZoftThing <TT> &cz)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              ampl (az), cent (cz)
          { }

        ZoftThing <TT> &Amplitude ()
          { return ampl; }
        ZoftThing <TT> &Center ()
          { return cent; }

        i64 Inhale (i64 ratch, f64 thyme)  override
          { ampl . Inhale (ratch, thyme);
            cent . Inhale (ratch, thyme);
            TT v = cent.val
              +  ampl.val * (2.0 * UnifMultiRand <TT> () - TT (1.0));
            this -> PuppeteerHosts (v);
            return 0;
          }
      };


  using GutsTyp = ZGuts <T>;

  GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &a)
    { return new ZGuts <T> (a); }
  static ZGuts <T> *NewGuts (const T &a, const T &c)
    { return new ZGuts <T> (a, c); }
  static ZGuts <T> *NewGuts (const ZoftThing <T> &az, const ZoftThing <T> &cz)
    { return new ZGuts <T> (az, cz); }

  RandZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  RandZoft (const T &a)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a)); }
  RandZoft (const T &a, const T &c)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a, c)); }
  RandZoft (const ZoftThing <T> &az)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (az, ZoftThing <T> ())); }
  RandZoft (const ZoftThing <T> &az,
            const ZoftThing <T> &cz)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (az, cz)); }

  RandZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }


  ZoftThing <T> &Amplitude ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Amplitude ();
      throw;
    }
  ZoftThing <T> &Center ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Center ();
      throw;
    }
};



using RandFloat = RandZoft <f64>;
using RandVect = RandZoft <Vect>;
using RandColor = RandZoft <ZeColor>;


}  //  so long; farewell; auf wiedersehen; goodbye... namespace zeugma


#endif
