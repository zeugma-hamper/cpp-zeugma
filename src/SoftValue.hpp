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

  SoftValue ();
  explicit SoftValue (V const &_v);

  SoftValue &operator= (V const &_v);

  bool operator== (SoftValue const &_v) const;

  operator ValueType & ();
  operator ValueType const & () const noexcept;

  ValueType &GetValue ();

  ValueType const &GetValue () const;

  void Set (ValueType const &_v);
  void Set (ValueType &&_v);

  void ClearDirty ();

  void SetDirty (bool _tf);

  bool IsDirty () const noexcept;

  ValueType m_value;
  bool m_dirty_flag;
};

template<typename V, typename AT>
struct AnimSoftValue : public SoftValue<V>
{
  using ValueType = typename SoftValue<V>::ValueType;
  using AnimationType = AT;

  AnimSoftValue ();
  explicit AnimSoftValue (V const &_v);

  ~AnimSoftValue ();

  void SetAnimation (AnimationType *_anim);
  AnimationType *GetAnimation () const;

  AnimationType *m_animation;
};

template<typename V>
SoftValue<V>::SoftValue ()
  : m_value {},
    m_dirty_flag {false}
{}

template<typename V>
SoftValue<V>::SoftValue (V const &_v)
  : m_value {_v},
    m_dirty_flag {true}
{ }

template<typename V>
SoftValue<V> &SoftValue<V>::operator= (V const &_v)
{
  m_value = _v;
  m_dirty_flag = true;
}

template<typename V>
bool SoftValue<V>::operator== (SoftValue const &_v) const
{
  return m_value == _v;
}

template<typename V>
SoftValue<V>::operator SoftValue<V>::ValueType & ()
{
  return m_value;
}

template<typename V>
SoftValue<V>::operator SoftValue<V>::ValueType const & () const noexcept
{
  return m_value;
}

template<typename V>
typename SoftValue<V>::ValueType &SoftValue<V>::GetValue ()
{
  return m_value;
}

template<typename V>
typename SoftValue<V>::ValueType const &SoftValue<V>::GetValue () const
{
  return m_value;
}

template<typename V>
void SoftValue<V>::Set (ValueType const &_v)
{
  m_value = _v;
  SetDirty (true);
}

template<typename V>
void SoftValue<V>::Set (ValueType &&_v)
{
  m_value = std::move (_v);
  SetDirty (true);
}

template<typename V>
void SoftValue<V>::ClearDirty ()
{
  m_dirty_flag = false;
}

template<typename V>
void SoftValue<V>::SetDirty (bool _tf)
{
  m_dirty_flag = _tf;
}

template<typename V>
bool SoftValue<V>::IsDirty () const noexcept
{
  return m_dirty_flag;
}


template<typename V, typename AT>
AnimSoftValue<V, AT>::AnimSoftValue ()
  : SoftValue<V> {},
    m_animation  {nullptr}
{}

template<typename V, typename AT>
AnimSoftValue<V, AT>::AnimSoftValue (V const &_v)
  : SoftValue<V> {_v},
    m_animation  {nullptr}
{ }

template<typename V, typename AT>
AnimSoftValue<V, AT>::~AnimSoftValue ()
{
  if (m_animation)
    m_animation->SetFinished ();

  m_animation = nullptr;
}

template<typename V, typename AT>
void AnimSoftValue<V, AT>::SetAnimation (AnimationType *_anim)
{
  if (m_animation)
    {
      m_animation->SetFinished ();
      m_animation = nullptr;
    }

  m_animation = _anim;
}

template<typename V, typename AT>
typename AnimSoftValue<V, AT>::AnimationType *
AnimSoftValue<V, AT>::GetAnimation () const
{
  return m_animation;
}


}

#endif //NOT_FIRM_AND_NOT_HARD
