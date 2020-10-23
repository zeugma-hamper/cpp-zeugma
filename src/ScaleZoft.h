
#ifndef SCALE_ZOFT_WITH_AN_IDENTIFIED_THUMB_ON_THE_PAN
#define SCALE_ZOFT_WITH_AN_IDENTIFIED_THUMB_ON_THE_PAN


#include "ZoftThing.h"


namespace charm  {


template <typename T>
class ScaleZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts <TT>
      { public:
        ZoftThing <TT> scalee;
        ZoftFloat scaler;

        ZGuts ()  :  ZoftThing<TT>::template ZGuts <TT> (),
                     scalee (TT (1.0)), scaler (1.0)
          { }
        ZGuts (const TT &lee, f64 ler)
           :  ZoftThing<TT>::template ZGuts <TT> (),
              scalee (lee), scaler (ler)
          { }
        ZGuts (const ZoftThing <TT> &leez, const ZoftFloat &lerz)
           :  ZoftThing<TT>::template ZGuts <TT> (),
              scalee (leez), scaler (lerz)
          { }

        ZoftThing <TT> &Scalee ()
          { return scalee; }
        ZoftFloat &Scaler ()
          { return scaler; }

        i64 Inhale (i64 ratch, f64 thyme)
          { scalee . Inhale (ratch, thyme);
            scaler . Inhale (ratch, thyme);
            TT v = scalee.val * scaler.val;
            this -> PuppeteerHosts (v);
            return 0;
          }
      };


  using GutsTyp = ZGuts <T>;

  inline GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &lee, f64 ler)
    { return new ZGuts <T> (lee, ler); }
  static ZGuts <T> *NewGuts (const ZoftThing <T> &leez, const ZoftFloat &lerz)
    { return new ZGuts <T> (leez, lerz); }

  ScaleZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  ScaleZoft (const T &lee, f64 ler)  :  ZoftThing <T> (lee * ler)
    { this -> InstallGuts (NewGuts (lee, ler)); }
  ScaleZoft (const ZoftThing <T> &leez, const ZoftFloat &lerz)
     :  ZoftThing <T> (leez.val * lerz.val)
    { this -> InstallGuts (NewGuts (leez, lerz)); }

  ScaleZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }

  ZoftThing <T> &Scalee ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Scalee ();
      throw;
    }
  ZoftFloat &Scaler ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Scaler ();
      throw;
    }
};



using ScaleFloat = ScaleZoft <f64>;
using ScaleVect = ScaleZoft <Vect>;


}  // that'd be about all she wrote for namespace charm (thanks, 'her')


#endif
