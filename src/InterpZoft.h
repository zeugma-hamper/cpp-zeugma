
#ifndef INTERP_ZOFT_IS_NEITHER_HERE_NOR_THERE_EXCEPT_FOR_VERY_BRIEFLY
#define INTERP_ZOFT_IS_NEITHER_HERE_NOR_THERE_EXCEPT_FOR_VERY_BRIEFLY


#include "ZoftThing.h"
#include "LatchZoft.h"
#include "interp-funcs.h"


namespace zeugma  {


template <typename T>
class InterpZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        ZoftThing <TT> pnt_a;
        ZoftThing <TT> pnt_b;
        f64 interp_time;
        InterpFuncs::INTERP_FUNC interp_funq;
        f64 start_time, recent_time;
        bool natal, replete;

        ZGuts ()
           :  ZoftThing<TT>::template ZGuts<TT> (),
              pnt_a (TT (0.0)), pnt_b (TT (0.0)),
              interp_time (1.0), interp_funq (InterpFuncs::ASYMP)
          { Commence (); }
        ZGuts (const TT &v)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              pnt_a (v), pnt_b (v),
              interp_time (1.0), interp_funq (InterpFuncs::ASYMP)
          { Commence (); }
        ZGuts (const TT &va, const TT &vb, f64 terpt = 1.0)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              pnt_a (va), pnt_b (vb),
              interp_time (terpt), interp_funq (InterpFuncs::ASYMP)
          { Commence (); }
        ZGuts (const ZoftThing <TT> &az,
               const ZoftThing <TT> &bz, f64 terpt = 1.0)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              pnt_a (az), pnt_b (bz),
              interp_time (terpt), interp_funq (InterpFuncs::ASYMP)
          { Commence (); }

        ZoftThing <TT> &PointA ()
          { return pnt_a; }
        ZoftThing <TT> &PointB ()
          { return pnt_b; }

        f64 InterpTime ()  const
          { return interp_time; }
        i64 SetInterpTime (f64 t)
          { if (t < 0.0)
              { interp_time = 0.0;
                return -1;
              }
            interp_time = t;
            return 0;
          }

        InterpFuncs::INTERP_FUNC InterpFunc ()  const
          { return interp_funq; }
        void SetInterpFunc (InterpFuncs::INTERP_FUNC ifnq)
          { interp_funq = ifnq; }

        void Commence ()
          { natal = true;
            replete = false;
            recent_time = 0.0;
            start_time = Zoft::zeit . CurTime ();
          }

        void Pause ()
          { recent_time = -fabs (recent_time); }
        void Continue ()
          { recent_time = fabs (recent_time); }

        void Finish ()
          { recent_time = interp_time; }

        void Reverse ()
          { ZoftThing <TT> tmp = pnt_a;
            pnt_a . BecomeLike (pnt_b);
            pnt_b . BecomeLike (tmp);
          }

        void ProceedTo (const ZoftThing <TT> &next_b)
          { pnt_a . BecomeLike (pnt_b);
            pnt_b . BecomeLike (next_b);
          }

        i64 Inhale (i64 ratch, f64 thyme)
          { pnt_a . Inhale (ratch, thyme);
            pnt_b . Inhale (ratch, thyme);
            if (natal)
              { f64 dt = Zoft::zeit . CurTime () - start_time;
                start_time = thyme - dt;
                natal = false;
              }
            if (recent_time  >=  interp_time)
              { this -> PuppeteerHosts (pnt_b.val);
                replete = true;  // use this to short-circuit next inhale?
                return 0;
              }
            recent_time = thyme - start_time;
            f64 t = interp_funq (recent_time / interp_time);
            if (t  >  1.0)
              t = 1.0;
            TT v ((1.0 - t) * pnt_a  +  t * pnt_b);
            this -> PuppeteerHosts (v);
            return 0;
          }
      };

  using GutsTyp = ZGuts <T>;

  GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &v)
    { return new ZGuts <T> (v); }
  static ZGuts <T> *NewGuts (const T &va, const T &vb, f64 terpt = 1.0)
    { return new ZGuts <T> (va, vb, terpt); }
  static ZGuts <T> *NewGuts (const ZoftThing <T> &az,
                             const ZoftThing <T> &bz, f64 terpt)
    { return new ZGuts <T> (az, bz, terpt); }

  InterpZoft ()  :  ZoftThing <T> (T (0.0))
    { this -> InstallGuts (NewGuts ()); }
  InterpZoft (const T &v)  :  ZoftThing <T> (v)
    { this -> InstallGuts (NewGuts (v)); }
  InterpZoft (const T &va, const T &vb, f64 terpt = 1.0)  :  ZoftThing <T> (va)
    { this -> InstallGuts (NewGuts (va, vb, terpt)); }
  InterpZoft (const ZoftThing <T> &va,
              const ZoftThing <T> &vb, f64 terpt = 1.0)  :  ZoftThing <T> (va)
    { this -> InstallGuts (NewGuts (va, vb, terpt)); }

  InterpZoft &operator = (const T &v)
    { ZoftThing<T>::Set (v);  return *this; }

  bool Replete ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g->replete;
      return false;
    }

  bool Commence ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Commence ();  return true; }
      return false;
    }

  bool Pause ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Pause ();  return true; }
      return false;
    }

  bool Continue ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Continue ();  return true; }
      return false;
    }

  bool Finish ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Finish ();  return true; }
      return false;
    }

  bool Reverse ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Reverse ();  return true; }
      return false;
    }

  bool ProceedTo (const ZoftThing <T> &next_bz)
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> ProceedTo (next_bz);  return true; }
      return false;
    }
  bool ProceedTo (const T &next_b)
    { ZoftThing <T> nbz (next_b);  return ProceedTo (nbz); }

  bool Set (const T &p)
    { return ProceedTo (p)  &&  Commence (); }

  bool SetHard (const T &p)
    { if (ProceedTo (p)  &&  Finish ())
        { GutsIfOrigType () -> PuppeteerHosts (p);
          return true;
        }
      return false;
    }

  ZoftThing <T> &PointA ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> PointA ();
      throw;
    }
  ZoftThing <T> &PointB ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> PointB ();
      throw;
    }

  f64 InterpTime ()  const
  { if (GutsTyp *g = GutsIfOrigType ())
      return g -> InterpTime ();
    throw;
  }
  bool SetInterpTime (f64 t)
  { if (GutsTyp *g = GutsIfOrigType ())
      { g -> SetInterpTime (t);  return true; }
    return false;
  }

  InterpFuncs::INTERP_FUNC InterpFunc ()  const
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> InterpFunc ();
      throw;
    }
  bool SetInterpFunc (InterpFuncs::INTERP_FUNC ifnq)
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> SetInterpFunc (ifnq);  return true; }
      return false;
    }
};



using InterpFloat = InterpZoft <f64>;
using InterpVect = InterpZoft <Vect>;
using InterpColor = InterpZoft <ZeColor>;


}  // et tu, bjarne? then fall, namespace zeugma!


#endif
