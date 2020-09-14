#ifndef FROM_POINT_A_TO_POINT_B
#define FROM_POINT_A_TO_POINT_B

#include "base_types.hpp"

namespace charm
{

class AnimationSystem;

enum class State
{
  Start,
  Continuing,
  Finished
};

class Animation
{
 public:
  using id = u64;

  Animation ();
  virtual ~Animation ();

  id GetId () const;

  State GetState () const;

  void SetFinished ();

  State Update (f64 timestamp, f64 delta, animation_step _step);

  virtual State DoUpdate (f64 timestamp, f64 delta, animation_step _step) = 0;

 protected:
  AnimationSystem *m_system; //non-owning
  id m_id;
  State m_state;
  animation_step m_last_step;
};

template<typename SV>
class SoftAnimation : public Animation
{
 public:
  using SoftValType = SV;

  SoftAnimation ()
    : SoftAnimation {nullptr}
  { }

  explicit SoftAnimation (SoftValType *_sv)
    : Animation {},
      m_value {_sv}
  { }

  ~SoftAnimation () override
  {
    if (m_value)
      m_value->SetAnimation (nullptr);

    m_value = nullptr;
  }

  SoftValType *GetSoftValue () const
  {
    return m_value;
  }

  void SetSoftValue (SoftValType *_sv)
  {
    m_value = _sv;
  }

 protected:
  SoftValType *m_value;
};

}

#endif //FROM_POINT_A_TO_POINT_B
