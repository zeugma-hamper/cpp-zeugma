
#ifndef THYME_IS_ON_MY_SIDE
#define THYME_IS_ON_MY_SIDE

#include <base_types.hpp>

#include <chrono>
#include <string>
#include <string_view>

#include <stdio.h>


namespace zeugma  {

/*
  BlockTimer can give you a quick measurement about how long a block
  takes to execute. It can also be used manually, by calling
  `StopTimer` directly.
 */
#if 1

struct BlockTimer
{
  using clock_t = std::chrono::high_resolution_clock;
  using timepoint_t = clock_t::time_point;
  using seconds_t = std::chrono::duration<f64>;

  explicit BlockTimer (std::string_view _view)
    : descriptor (_view),
      start_time (timepoint_t::min ()),
      end_time (timepoint_t::min ())
  {
    start_time = clock_t::now ();
  }

  void StopTimer ()
  {
    if (end_time == timepoint_t::min ())
      end_time = clock_t::now ();
  }

  ~BlockTimer ()
  {
    StopTimer ();
    printf ("%s: %0.4f ms\n", descriptor.c_str (),
            seconds_t (end_time - start_time).count () * 1000.0);
  }

  std::string descriptor;
  timepoint_t start_time;
  timepoint_t end_time;
};

#else

struct BlockTimer
{
  using clock_t = std::chrono::high_resolution_clock;
  using timepoint_t = clock_t::time_point;
  using seconds_t = std::chrono::duration<f64>;

  explicit BlockTimer (std::string_view)
  { }

  void StopTimer ()
  { }

  ~BlockTimer ()
  { }

  std::string descriptor;
  timepoint_t start_time;
  timepoint_t end_time;
};

#endif


}


#endif  //THYME_IS_ON_MY_SIDE
