#ifndef NOT_FIRM_AND_NOT_HARD
#define NOT_FIRM_AND_NOT_HARD

#include "Animation.hpp"
#include "base_types.hpp"

#include <utility>

// dislike the names? me too!

namespace charm
{

class Animation;

template<typename V>
struct SoftValue
{
  using ValueType = V;

  SoftValue ()
    : m_value {},
      m_dirty_flag {false}
  {}

  explicit SoftValue (V const &_v)
    : m_value {_v},
      m_dirty_flag {true}
  { }

  SoftValue &operator= (V const &_v)
  {
    m_value = _v;
    m_dirty_flag = true;
  }

  bool operator== (SoftValue const &_v) const
  {
    return m_value == _v;
  }

  operator ValueType & ()
  {
    return m_value;
  }

  operator ValueType const & () const noexcept
  {
    return m_value;
  }

  ValueType &GetValue ()
  {
    return m_value;
  }

  ValueType const &GetValue () const
  {
    return m_value;
  }

  void Set (ValueType const &_v)
  {
    m_value = _v;
    SetDirty (true);
  }

  void Set (ValueType &&_v)
  {
    m_value = std::move (_v);
    SetDirty (true);
  }

  void ClearDirty ()
  {
    m_dirty_flag = false;
  }

  void SetDirty (bool _tf)
  {
    m_dirty_flag = _tf;
  }

  bool IsDirty () const noexcept
  {
    return m_dirty_flag;
  }

  ValueType m_value;
  bool m_dirty_flag;
};

template<typename V, typename AT>
struct AnimSoftValue : public SoftValue<V>
{
  using ValueType = typename SoftValue<V>::ValueType;
  using AnimationType = AT;

  AnimSoftValue ()
    : SoftValue<V> {},
      m_animation  {nullptr}
  {}

  explicit AnimSoftValue (V const &_v)
    : SoftValue<V> {_v},
      m_animation  {nullptr}
  { }

  ~AnimSoftValue ()
  {
    if (m_animation)
      m_animation->SetFinished ();

    m_animation = nullptr;
  }

  void SetAnimation (AnimationType *_anim)
  {
    if (m_animation)
      {
        m_animation->SetFinished ();
        m_animation = nullptr;
      }

    m_animation = _anim;
  }

  AnimationType *GetAnimation () const
  {
    return m_animation;
  }

  AnimationType *m_animation;
};

}

#endif //NOT_FIRM_AND_NOT_HARD
