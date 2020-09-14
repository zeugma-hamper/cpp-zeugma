#include "Animation.hpp"

namespace charm
{

Animation::Animation ()
  : m_id {0u},
    m_state {State::Start},
    m_last_step {0u}
{ }

Animation::~Animation ()
{ }

Animation::id Animation::GetId () const
{
  return m_id;
}

State Animation::GetState () const
{
  return m_state;
}

void Animation::SetFinished ()
{
  m_state = State::Finished;
}

State Animation::Update (f64 _timestamp, f64 _delta, animation_step _step)
{
  if (m_state == State::Finished || m_last_step == _step)
    return m_state;

  return (m_state = DoUpdate (_timestamp, _delta, _step));
}

}
