
#include "MotherTime.h"

#include <chrono>


using namespace std;


namespace zeugma  {


// that's "one over a billion", friends...
static f64 OOAB = (1.0 / 1000000000.0);


u64 NANOSECS_OF_NOW ()
{ auto nowish = chrono::steady_clock::now () . time_since_epoch ();
  auto nanos = chrono::duration_cast <chrono::nanoseconds> (nowish);
  return nanos . count ();
}


f64 MotherTime::AbsoluteTime ()
{ return OOAB * NANOSECS_OF_NOW (); }



MotherTime::MotherTime ()  :  secs_per_sec (1.0), chrono_freeze (false)
{ ZeroTime (); }


MotherTime::MotherTime (f64 start_time)
  :  secs_per_sec (1.0), chrono_freeze (false)
{ SetTime (start_time); }


MotherTime::MotherTime (const MotherTime &otra)
 :  nowesque_time (otra.nowesque_time),
    prev_nanosecs (otra.prev_nanosecs),
    secs_per_sec (otra.secs_per_sec),
    chrono_freeze (otra.chrono_freeze)
{ }


f64 MotherTime::CurTime ()
{ if (chrono_freeze)
    return nowesque_time;
  return (nowesque_time += guts_ElapsedOffset ());
}


f64 MotherTime::CurTimeGlance ()  const
{ if (chrono_freeze)
    return nowesque_time;
  return (nowesque_time + guts_ElapsedOffsetGlance ());
}


f64 MotherTime::DeltaTime ()
{ if (chrono_freeze)
    return 0.0;
  f64 dt = guts_ElapsedOffset ();
  nowesque_time += dt;
  return dt;
}


f64 MotherTime::DeltaTimeGlance ()  const
{ if (chrono_freeze)
    return 0.0;
  return guts_ElapsedOffsetGlance ();
}


f64 MotherTime::SetTime (f64 t)
{ guts_ElapsedOffset ();
  return (nowesque_time = t);
}


f64 MotherTime::ShiftTime (f64 dt)
{ guts_ElapsedOffset ();
  return (nowesque_time += dt);
}


f64 MotherTime::ZeroTime ()
{ guts_ElapsedOffset ();
  return (nowesque_time = 0.0);
}


f64 MotherTime::SetTimePausedness (bool tp)
{ if (chrono_freeze  ==  tp)
    return nowesque_time;
  f64 cur = CurTime ();
  chrono_freeze = tp;
  return cur;
}


f64 MotherTime::SetTimeFlowRate (f64 sps)
{ f64 cur = CurTime ();
  secs_per_sec = sps;
  return cur;
}



f64 MotherTime::guts_ElapsedOffset ()
{ u64 cur_nsecs = NANOSECS_OF_NOW ();
  f64 diff_time = secs_per_sec * OOAB * (cur_nsecs - prev_nanosecs);
  prev_nanosecs = cur_nsecs;
  return diff_time;
}

f64 MotherTime::guts_ElapsedOffsetGlance ()  const
{ u64 cur_nsecs = NANOSECS_OF_NOW ();
  return secs_per_sec * OOAB * (cur_nsecs - prev_nanosecs);
}


}  // goes the way of all flesh, does namespace zeugma
