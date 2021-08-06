
#include "FrameTime.hpp"

#include <chrono>


namespace zeugma  {


static u64 now_in_nanos ()
{
  using nanos = std::chrono::nanoseconds;
  auto dur = std::chrono::high_resolution_clock().now ().time_since_epoch ();
  return std::chrono::duration_cast<nanos>(dur).count ();
}

FrameTime::FrameTime ()
  : m_current_time {0u},
    m_reset_time {now_in_nanos()},
    m_frame_delta {0u}
{}

void FrameTime::UpdateTime ()
{
  u64 const nin = now_in_nanos() - m_reset_time;

  m_frame_delta =  nin - m_current_time;
  m_current_time = nin;
}

void FrameTime::AdvanceTime (u64 _dt)
{
  m_current_time += _dt;
  m_frame_delta = _dt;
}

f64 FrameTime::GetCurrentTime () const
{
  f64 const ooab = 1.0/1000000000.0;
  return m_current_time * ooab;
}

f64 FrameTime::GetCurrentDelta () const
{
  f64 const ooab = 1.0/1000000000.0;
  return m_frame_delta * ooab;
}

f64 FrameTime::ResetTime () const
{
  f64 const ooab = 1.0/1000000000.0;
  return m_reset_time * ooab;
}

u64 FrameTime::GetCurrentTimeNS () const
{
  return m_current_time;
}

u64 FrameTime::GetCurrentDeltaNS () const
{
  return m_frame_delta;
}

u64 FrameTime::ResetTimeNS () const
{
  return m_reset_time;
}


}
