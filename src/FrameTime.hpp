#ifndef WANT_TO_USE_MOTHER_TIME
#define WANT_TO_USE_MOTHER_TIME

#include <base_types.hpp>

namespace charm
{
  class FrameTime
  {
   public:

    FrameTime ();

    void update_time ();
    void advance_time (u64 _dt);

    f64 current_time ()  const;
    f64 current_delta () const;
    f64 reset_time ()    const;

    u64 current_time_ns ()  const;
    u64 current_delta_ns () const;
    u64 reset_time_ns ()    const;

   protected:

    u64 m_current_time;
    u64 m_reset_time;
    u64 m_frame_delta;
  };
}

#endif //WANT_TO_USE_MOTHER_TIME
