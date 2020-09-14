#include "AnimationSystem.hpp"

#include "Animation.hpp"

namespace charm
{

static AnimationSystem *s_animation_system {nullptr};

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

void AnimationSystem::UpdateAnimations (double timestamp, double delta)
{
  using anim_iterator = std::vector<Animation *>::iterator;

  ++m_current_step;

  size_t const count = m_animations.size ();
  std::vector<anim_iterator> finished_animations;
  finished_animations.reserve (count >> 1);

  auto end = m_animations.end ();
  for (auto cur = m_animations.begin (); cur != end; ++cur)
  {
    if (State::Finished == (*cur)->Update (timestamp, delta, m_current_step))
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

void AnimationSystem::AddAnimation(Animation *_animation)
{
  m_animations.push_back(_animation);
}

void AnimationSystem::Initialize ()
{
  s_animation_system = new AnimationSystem;
}

void AnimationSystem::ShutDown ()
{
  delete s_animation_system;
  s_animation_system = nullptr;
}

AnimationSystem *AnimationSystem::GetSystem ()
{
  return s_animation_system;
}

}
