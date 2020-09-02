
#ifndef NO_TIME_NO_TIME
#define NO_TIME_NO_TIME

#include <cstdint>
#include <vector>

namespace charm
{

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

  void set_finished ();

  State update (double timestamp, double delta, AnimationSystem::step _step);

  virtual State do_update (double timestamp, double delta) = 0;

 protected:
  AnimationSystem *m_system; //non-owning
  id m_id;
  State m_state;
  AnimationSystem::step m_last_step;
};

template<typename SV>
class SoftAnimation : public Animation
{
 public:
  using SoftVal = SV;

  SoftAnimation ()
    : SoftAnimation {nullptr}
  { }

  SoftAnimation (SoftVal *_sv)
    : Animation {},
      m_value {_sv}
  {
  }

  ~SoftAnimation () override
  {
    if (m_value)
      m_value->set_animation (nullptr);

    m_value = nullptr;
  }

  SoftVal *get_soft_value () const
  {
    return m_value;
  }

  void set_soft_value (SoftVal *_sv)
  {
    m_value = _sv;
  }

 protected:
  SoftVal *m_value;
};

}
#endif //NO_TIME_NO_TIME
