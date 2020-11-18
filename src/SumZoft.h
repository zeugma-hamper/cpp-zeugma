
#ifndef SUM_ZOFT_IS_QUITE_LIKELY_LESS_THAN_ITSELF
#define SUM_ZOFT_IS_QUITE_LIKELY_LESS_THAN_ITSELF


#include "ZoftThing.h"


namespace charm  {


template <typename T>
class SumZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        ZoftThing <TT> a;
        ZoftThing <TT> b;

        ZGuts ()  :  ZoftThing<TT>::template ZGuts<TT> (),
           a (TT (0.0)), b (TT (0.0))
          { }
        ZGuts (const TT &aa)
           :  ZoftThing<TT>::template ZGuts<TT> (aa), a (aa), b (TT (0.0))
          { }
        ZGuts (const TT &aa, const TT &bb)
           :  ZoftThing<TT>::template ZGuts<TT> (), a (aa), b (bb)
          { }
        ZGuts (const ZoftThing <TT> &aaz, const ZoftThing <TT> &bbz)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              a (aaz), b (bbz)
          { }

        ZoftThing <TT> &SummandA ()
          { return a; }
        ZoftThing <TT> &SummandB ()
          { return b; }

        i64 Inhale (i64 ratch, f64 thyme)  override
          { a . Inhale (ratch, thyme);
            b . Inhale (ratch, thyme);
            TT v = a.val + b.val;
            this -> PuppeteerHosts (v);
            return 0;
          }
      };


  using GutsTyp = ZGuts <T>;

  GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &aa)
    { return new ZGuts <T> (aa); }
  static ZGuts <T> *NewGuts (const T &aa, const T &bb)
    { return new ZGuts <T> (aa, bb); }
  static ZGuts <T> *NewGuts (const ZoftThing <T> &aaz, const ZoftThing <T> &bbz)
    { return new ZGuts <T> (aaz, bbz); }

  SumZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  SumZoft (const T &aa)  :  ZoftThing <T> (aa)
    { this -> InstallGuts (NewGuts (aa)); }
  SumZoft (const T &aa, const T &bb)  :  ZoftThing <T> (aa + bb)
    { this -> InstallGuts (NewGuts (aa, bb)); }
  SumZoft (const ZoftThing <T> &aaz,
           const ZoftThing <T> &bbz)  :  ZoftThing <T> (aaz.val + bbz.val)
    { this -> InstallGuts (NewGuts (aaz, bbz)); }

  SumZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }

  ZoftThing <T> &SummandA ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> SummandA ();
      throw;
    }
  ZoftThing <T> &SummandB ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> SummandB ();
      throw;
    }
};



using SumFloat = SumZoft <f64>;
using SumVect = SumZoft <Vect>;
using SumColor = SumZoft <ZeColor>;


}  // end o' the line for namespace charm


#endif
