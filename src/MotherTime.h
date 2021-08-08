
//
// (c) treadle & loam, provisioners llc
//

#ifndef MOTHER_TIME_PULLS_ALL_THE_STRINGS
#define MOTHER_TIME_PULLS_ALL_THE_STRINGS


//#include "slappy-types.h"
#include "base_types.hpp"


namespace zeugma  {


class MotherTime
{ public:
  f64 nowesque_time;
  u64 prev_nanosecs;
  f64 secs_per_sec;
  bool chrono_freeze;

  static f64 AbsoluteTime ();

  MotherTime ();
  MotherTime (f64 start_time);
  MotherTime (const MotherTime &otra);

  f64 CurTime ();
  f64 CurTimeGlance ()  const;

  f64 DeltaTime ();
  f64 DeltaTimeGlance ()  const;

  f64 SetTime (f64 t);
  f64 ShiftTime (f64 delta);
  f64 ZeroTime ();

  bool TimePausedness ()  const
    { return chrono_freeze; }
  f64 SetTimePausedness (bool tp);
  f64 PauseTime ()
    { return SetTimePausedness (true); }
  f64 UnpauseTime ()
    { return SetTimePausedness (false); }
  f64 ToggleTimePausedness ()
    { return SetTimePausedness (TimePausedness ()  ?  false  :  true); }

  f64 TimeFlowRate ()  const
    { return secs_per_sec; }
  f64 SetTimeFlowRate (f64 sps);

 protected:
  f64 guts_ElapsedOffset ();
  f64 guts_ElapsedOffsetGlance ()  const;
};


}  // quoth namespace zeugma: we who are about to die sa...


#endif
