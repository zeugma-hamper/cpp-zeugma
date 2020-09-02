#include "AnimationSystem.hpp"

namespace charm
{

AnimationSystem::AnimationSystem ()
  : m_current_step {0u}
{
}

AnimationSystem::~AnimationSystem ()
{
  for (Animation *anim : m_animations)
    delete anim;
  for (Animation *anim : m_finished)
    delete anim;

  m_animations.clear ();
}

void AnimationSystem::update_animations (double timestamp, double delta)
{
  using anim_iterator = std::vector<Animation *>::iterator;

  ++m_current_step;

  size_t const count = m_animations.size ();
  std::vector<anim_iterator> finished_animations;
  finished_animations.reserve (count >> 1);

  auto end = m_animations.end ();
  for (auto cur = m_animations.begin (); cur != end; ++cur)
  {
    if (State::Finished == (*cur)->update (timestamp, delta, m_current_step))
      finished_animations.push_back (cur);
  }

  size_t const fin_count = finished_animations.size ();
  for (size_t i = 1; i <= fin_count; ++i)
    {
      size_t const index = fin_count - i;
      delete *finished_animations[index];
      m_animations.erase (finished_animations[index]);
    }
}

Animation::Animation ()
  : m_id {0u},
    m_state {State::Start},
    m_last_step {0u}
{ }

Animation::~Animation ()
{ }

Animation::id Animation::get_id () const
{
  return m_id;
}

State Animation::get_state () const
{
  return m_state;
}

State Animation::update (double _timestamp, double _delta, AnimationSystem::step _step)
{
  if (m_last_step == _step)
    return m_state;

  return (m_state = do_update (_timestamp, _delta));
}

}
