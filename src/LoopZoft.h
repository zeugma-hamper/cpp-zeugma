
#ifndef LOOP_ZOFT_PLAYS_ENDLESSLY_BUT_IRRITATES_NOBODY
#define LOOP_ZOFT_PLAYS_ENDLESSLY_BUT_IRRITATES_NOBODY


#include "ZoftThing.h"


namespace charm  {


template <typename T>
class LoopZoft  :  public ZoftThing <T>
{ public:

  template <typename TT>
    class ZGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        TT prev_val;
        ZoftThing <TT> start_val;
        ZoftThing <TT> dx_dt;
        ZoftFloat loop_dur;
        f64 prev_time, loop_time;
        bool natal;

        ZGuts ()  :  ZoftThing<TT>::template ZGuts<TT> (),
                     start_val (TT (0.0)), dx_dt (TT (1.0)), loop_dur (0.0),
                     loop_time (0.0), natal (true)
          { }
        ZGuts (const TT &start, const TT &slope, f64 ldur = 0.0)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              start_val (start), dx_dt (slope), loop_dur (ldur),
              loop_time (0.0), natal (true)
          { }
        ZGuts (const TT &start, const ZoftThing <TT> &slopez, f64 ldur = 0.0)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              start_val (start), dx_dt (slopez), loop_dur (ldur),
              loop_time (0.0), natal (true)
          { }
        ZGuts (const TT &start, const TT &slope, const ZoftFloat &ldurz)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              start_val (start), dx_dt (slope), loop_dur (ldurz),
              loop_time (0.0), natal (true)
          { }
        ZGuts (const TT &start, const ZoftThing <TT> &slopez,
               const ZoftFloat &ldurz)
           :  ZoftThing<TT>::template ZGuts<TT> (),
              start_val (start), dx_dt (slopez), loop_dur (ldurz),
              loop_time (0.0), natal (true)
          { }

        ZoftThing <TT> &StartVal ()
          { return start_val; }
        ZoftThing <TT> &DXDT ()
          { return dx_dt; }
        ZoftFloat &LoopDuration ()
          { return loop_dur; }

        void Restart ()
          { natal = true; }

        i64 Inhale (i64 ratch, f64 thyme)
          { if (natal)
              { prev_val = start_val;
                prev_time = thyme;
                loop_time = 0.0;
                natal = false;
              }
            start_val . Inhale (ratch, thyme);
            dx_dt . Inhale (ratch, thyme);
            loop_dur . Inhale (ratch, thyme);
            f64 dt = thyme - prev_time;
            prev_time = thyme;
            loop_time += dt;
            TT cur_val = prev_val + dt * dx_dt.val;
            if (loop_dur.val > 0.0)
              { while (loop_time > loop_dur.val)
                  loop_time -= loop_dur.val;
                cur_val = start_val.val  +  loop_time * dx_dt.val;
              }
            this -> PuppeteerHosts (cur_val);
            prev_val = cur_val;
            return 0;
          }
      };


  using GutsTyp = ZGuts <T>;

  inline GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  static ZGuts <T> *NewGuts ()
    { return new ZGuts <T> (); }
  static ZGuts <T> *NewGuts (const T &start, const T &slope,
                             f64 ldur = 0.0)
    { return new ZGuts <T> (start, slope, ldur); }
  static ZGuts <T> *NewGuts (const T &start, const ZoftThing <T> &slopez,
                             f64 ldur = 0.0)
    { return new ZGuts <T> (start, slopez, ldur); }
  static ZGuts <T> *NewGuts (const T &start, const T &slope,
                             const ZoftFloat &ldurz)
    { return new ZGuts <T> (start, slope, ldurz); }
  static ZGuts <T> *NewGuts (const T &start, const ZoftThing <T> &slopez,
                             const ZoftFloat &ldurz)
    { return new ZGuts <T> (start, slopez, ldurz); }

  LoopZoft ()  :  ZoftThing <T> ()
    { this -> InstallGuts (NewGuts ()); }
  LoopZoft (const T &start, const T &slope,
            f64 ldur = 0.0)  :  ZoftThing <T> (start)
    { this -> InstallGuts (NewGuts (start, slope, ldur)); }
  LoopZoft (const T &start, const ZoftThing <T> &slopez,
            f64 ldur = 0.0)  :  ZoftThing <T> (start)
    { this -> InstallGuts (NewGuts (start, slopez, ldur)); }
  LoopZoft (const T &start, const T &slope,
            const ZoftFloat &ldurz)  :  ZoftThing <T> (start)
    { this -> InstallGuts (NewGuts (start, slope, ldurz)); }
  LoopZoft (const T &start, const ZoftThing <T> &slopez,
            const ZoftFloat &ldurz)  :  ZoftThing <T> (start)
    { this -> InstallGuts (NewGuts (start, slopez, ldurz)); }

  ZoftThing <T> &StartVal ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> StartVal ();
      throw;
    }
  ZoftThing <T> &DXDT ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> DXDT ();
      throw;
    }
  ZoftThing <T> &LoopDuration ()
    { if (GutsTyp *g = GutsIfOrigType ())
        return g -> LoopDuration ();
      throw;
    }

  bool Restart ()
    { if (GutsTyp *g = GutsIfOrigType ())
        { g -> Restart ();
          return true;
        }
      return false;
    }
};



using LoopFloat = LoopZoft <f64>;
using LoopVect = LoopZoft <Vect>;


}  // this has gone far enough, namespace charm. now git. scram.


#endif
