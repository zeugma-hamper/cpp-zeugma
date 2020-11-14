
#ifndef SINU_ZOFT_WIGGLES_IN_THE_CONGA_LINE_OF_LIFE
#define SINU_ZOFT_WIGGLES_IN_THE_CONGA_LINE_OF_LIFE


#include "ZoftThing.h"


namespace charm  {


template <typename T>
class SinuZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        ZoftThing <TT> ampl;
        ZoftFloat freq;
        ZoftThing <TT> cent;
        ZoftFloat phas;

        ZGuts ()  :  ZoftThing<TT>::template ZGuts<TT> (),
           ampl (TT (1.0)), freq (1.0), cent (TT (0.0)), phas (0.0)
          { }
        ZGuts (const TT &a)  :  ZoftThing<TT>::template ZGuts<TT> (),
           ampl (a), freq (1.0), cent (TT (0.0)), phas (0.0)
          { }
        ZGuts (const TT &a, f64 f)  :  ZoftThing<TT>::template ZGuts<TT> (),
           ampl (a), freq (f), cent (TT (0.0)), phas (0.0)
          { }
        ZGuts (const TT &a, f64 f, const TT &c, f64 p = 0.0)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              ampl (a), freq (f), cent (c), phas (p)
          { }

        ZoftThing <TT> &Amplitude ()
          { return ampl; }
        ZoftFloat &Frequency ()
          { return freq; }
        ZoftThing <TT> &Center ()
          { return cent; }
        ZoftFloat &Phase ()
          { return phas; }

        i64 Inhale (i64 ratch, f64 thyme)  override
          { ampl . Inhale (ratch, thyme);
            freq . Inhale (ratch, thyme);
            cent . Inhale (ratch, thyme);
            phas . Inhale (ratch, thyme);
            TT v = cent.val
              +  ampl.val * sin (fmod (freq * thyme + phas, 2.0 * M_PI));
            this -> PuppeteerHosts (v);
            return 0;
          }
      };


  using GutsTyp = ZGuts <T>;

  inline GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &a)
    { return new ZGuts <T> (a); }
  static ZGuts <T> *NewGuts (const T &a, f64 f)
    { return new ZGuts <T> (a, f); }
  static ZGuts <T> *NewGuts (const T &a, f64 f, const T &c)
    { return new ZGuts <T> (a, f, c); }
  static ZGuts <T> *NewGuts (const T &a, f64 f, const T &c, f64 p)
    { return new ZGuts <T> (a, f, c, p); }

  SinuZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  SinuZoft (const T &a)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a)); }
  SinuZoft (const T &a, f64 f)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a, f)); }
  SinuZoft (const T &a, f64 f, const T &c)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a, f, c)); }
  SinuZoft (const T &a, f64 f, const T &c, f64 p)  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts (a, f, c, p)); }

  SinuZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }


  ZoftThing <T> &Amplitude ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Amplitude ();
      throw;
    }
  ZoftFloat &Frequency ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Frequency ();
      throw;
    }
  ZoftThing <T> &Center ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Center ();
      throw;
    }
  ZoftFloat &Phase ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> Phase ();
      throw;
    }
};



using SinuFloat = SinuZoft <f64>;
using SinuVect = SinuZoft <Vect>;
using SinuColor = SinuZoft <ZeColor>;


}  //  so long; farewell; auf wiedersehen; goodbye... namespace charm


#endif
