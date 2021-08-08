
//
// (c) treadle & loam, provisioners llc
//

#ifndef WANT_TO_USE_MOTHER_TIME
#define WANT_TO_USE_MOTHER_TIME


#include <base_types.hpp>


namespace zeugma  {


class FrameTime
{
 public:

  FrameTime ();

  void UpdateTime  ();
  void AdvanceTime (u64 _dt);

  f64 GetCurrentTime  () const;
  f64 GetCurrentDelta () const;
  f64 ResetTime ()       const;

  u64 GetCurrentTimeNS  () const;
  u64 GetCurrentDeltaNS () const;
  u64 ResetTimeNS ()       const;

 protected:

  u64 m_current_time;
  u64 m_reset_time;
  u64 m_frame_delta;
};


}


#endif  //WANT_TO_USE_MOTHER_TIME
