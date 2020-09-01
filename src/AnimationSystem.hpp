
#ifndef NO_TIME_NO_TIME
#define NO_TIME_NO_TIME

#include <cstdint>
#include <vector>


class Animation;
class AnimationSystem;

enum class State
{
  Start,
  Continuing,
  Finished
};

class AnimationSystem
{
 public:
  using step = std::uint64_t;
  AnimationSystem ();
  ~AnimationSystem ();

  void update_animations (double timestamp, double delta);

 protected:
  std::vector<Animation *> m_animations;
  std::vector<Animation *> m_finished;
  step m_current_step;
};

class Animation
{
 public:
  using id = std::uint64_t;

  Animation ();
  virtual ~Animation ();

  id get_id () const;
  State get_state () const;

  State update (double timestamp, double delta, AnimationSystem::step _step);

  virtual State do_update (double timestamp, double delta) = 0;

 protected:
  AnimationSystem *m_system; //non-owning
  id m_id;
  State m_state;
  AnimationSystem::step m_last_step;
};


#endif //NO_TIME_NO_TIME
