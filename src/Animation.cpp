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

Animation::id Animation::get_id () const
{
  return m_id;
}

State Animation::get_state () const
{
  return m_state;
}

void Animation::set_finished ()
{
  m_state = State::Finished;
}

State Animation::update (f64 _timestamp, f64 _delta, animation_step _step)
{
  if (m_state == State::Finished || m_last_step == _step)
    return m_state;

  return (m_state = do_update (_timestamp, _delta, _step));
}

}
