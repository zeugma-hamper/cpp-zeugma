
#ifndef NO_TIME_NO_TIME
#define NO_TIME_NO_TIME

#include <base_types.hpp>
#include <vector>

namespace charm
{

class Animation;

class AnimationSystem
{
 public:
  AnimationSystem ();
  ~AnimationSystem ();

  void update_animations (f64 timestamp, f64 delta);

  void add_animation (Animation *_anim);

  static void initialize ();
  static void shut_down ();

  static AnimationSystem *get_system ();

 protected:
  std::vector<Animation *> m_animations;
  std::vector<Animation *> m_finished;
  animation_step m_current_step;
};


}
#endif //NO_TIME_NO_TIME
