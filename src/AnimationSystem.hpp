
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

  void UpdateAnimations (f64 timestamp, f64 delta);

  void AddAnimation (Animation *_anim);

  static void Initialize ();
  static void ShutDown ();

  static AnimationSystem *GetSystem ();

 protected:
  std::vector<Animation *> m_animations;
  std::vector<Animation *> m_finished;
  animation_step m_current_step;
};


}
#endif //NO_TIME_NO_TIME
