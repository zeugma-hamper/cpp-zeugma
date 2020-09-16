#ifndef SOFT_TYPES
#define SOFT_TYPES

#include <SoftValue.hpp>
#include <Animation.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


//Q: Why have Animation as a template parameter rather than
//   a concrete component of the subclass?
//A: I would like to have interpolation animations and values that
//   know and expect that sort of interpolation.

namespace charm
{
class Animation;

using FloatSoft = SoftValue<f32>;
using FloatAnim = AnimSoftValue<f32, Animation>;

using Vec3Soft = SoftValue<glm::vec3>;
using Vec3Anim = AnimSoftValue<glm::vec3, Animation>;

using QuatSoft = SoftValue<glm::quat>;
using QuatAnim = AnimSoftValue<glm::quat, Animation>;

template<typename SV>
class InterpolationAnimation : public SoftAnimation<SV>
{
 public:
  using SoftValueType = SV;
  using ValueType = typename SV::ValueType;
  using ParentType = SoftAnimation<SV>;

  InterpolationAnimation ()
    : SoftAnimation<SV> {},
      m_start_val {},
      m_goal_val {},
      m_duration {1.0f},
      m_start_time {-1.0f}
  {
    Animation::m_state = State::Start;
  }

  void SetStartValue (ValueType const &_val)
  {
    m_start_val = _val;
  }

  ValueType const &GetStartValue () const
  {
    return m_start_val;
  }

  void SetGoalValue (ValueType const &_val)
  {
    m_goal_val = _val;
  }

  ValueType const &GetGoalValue () const
  {
    return m_goal_val;
  }

  void SetInterpTime (f32 _duration)
  {
    m_duration = _duration;
  }

  f32 GetInterpTime () const
  {
    return m_duration;
  }

  State DoUpdate (f64 _timestamp, f64, animation_step)
  {
    // first run
    if (m_start_time < 0.0f || Animation::m_state == State::Start)
      {
        m_start_time = f32 (_timestamp);
        ParentType::m_value->SetValue (m_start_val);
        return State::Continuing;
      }

    // end or approximatly end of run
    f32 const time_smidge = 0.001f;
    if (_timestamp <= m_duration + m_start_time + time_smidge)
      {
        ParentType::m_value->SetValue (m_goal_val);
        return State::Finished;
      }

    //just linear interpolation now, but you can see that
    //any of those interp functions would work
    f64 const pct = (_timestamp - m_start_time) / m_duration;
    ValueType const v ((1.0 - pct) * m_start_val + pct * m_goal_val);
    ParentType::m_value->SetValue (v);
    return State::Continuing;
  }

 protected:
  ValueType m_start_val;
  ValueType m_goal_val;
  f32 m_duration;
  f32 m_start_time;
};

// distance should be nonnegative
template<typename T>
f32 cs_distance (T const &, T const &);
// direction to head determined by start and end points
// for linear interpolation
template<typename T>
T cs_direction (T const &_from, T const &_to);



template<typename SV>
class ConstantSpeedAnimation : public SoftAnimation<SV>
{
 public:
  using SoftValueType = SV;
  using ValueType = typename SV::ValueType;
  using ParentType = SoftAnimation<SV>;

  ConstantSpeedAnimation ()
    : SoftAnimation<SV> {},
      m_start_val {},
      m_goal_val {},
      m_units_per_sec {1.0f},
      m_start_time {-1.0f}
  {
    Animation::m_state = State::Start;
  }

  void SetStartValue (ValueType const &_val)
  {
    m_start_val = _val;
  }

  void SetGoalValue (ValueType const &_val)
  {
    m_goal_val = _val;
  }

  void SetSpeed (f32 _units_per_sec)
  {
    m_units_per_sec = _units_per_sec;
  }

  State DoUpdate (f64 _timestamp, f64, animation_step)
  {
    // first run
    if (m_start_time < 0.0f || Animation::m_state == State::Start)
      {
        m_start_time = f32 (_timestamp);
        ParentType::m_value->SetValue (m_start_val);
        return State::Continuing;
      }

    // the below is simplistic, but i think it illustrates the possibilities.
    // need soft sort of mag and interpolation functions here.
    f32 const total_dist = cs_distance (m_start_val, m_goal_val);
    ValueType const dir = cs_direction (m_start_val, m_goal_val);
    f64 const total_time_delta = _timestamp - m_start_time;

    if (total_dist <= m_units_per_sec * total_time_delta)
      {
        ParentType::m_value->SetValue (m_goal_val);
        return State::Finished;
      }

    ValueType const curr (m_start_val + dir * total_time_delta * m_units_per_sec);
    ParentType::m_value->SetValue (curr);
    return State::Continuing;
  }

 protected:
  ValueType m_start_val;
  ValueType m_goal_val;
  f32 m_units_per_sec;
  f32 m_start_time;
};

template<>
f32 cs_distance<f32> (f32 const &_a, f32 const &_b)
{
  return std::abs(_a - _b);
}

template<>
f32 cs_distance<glm::vec3> (glm::vec3 const &_a, glm::vec3 const &_b)
{
  return glm::distance(_a, _b);
}

template<>
f32 cs_distance<glm::vec4> (glm::vec4 const &_a, glm::vec4 const &_b)
{
  return glm::distance(_a, _b);
}


template<>
f32 cs_direction<f32> (f32 const &_from, f32 const &_to)
{
  return _from > _to ? -1.0f : 1.0f;
}

template<>
glm::vec3 cs_direction<glm::vec3> (glm::vec3 const &_from, glm::vec3 const &_to)
{
  return glm::normalize(_to - _from);
}

template<>
glm::vec4 cs_direction<glm::vec4> (glm::vec4 const &_from, glm::vec4 const &_to)
{
  return glm::normalize(_to - _from);
}

}

#endif //SOFT_TYPES
