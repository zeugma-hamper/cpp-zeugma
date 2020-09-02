#include "base_types.hpp"
#include <glm/glm.hpp>

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

  SoftValue (V const &_v)
    : m_value {_v},
      m_dirty_flag {false}
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

  ValueType &get_value ()
  {
    return m_value;
  }

  ValueType const &get_value () const
  {
    return m_value;
  }

  void clear_dirty ()
  {
    m_dirty_flag = false;
  }

  void set_dirty (bool _tf)
  {
    m_dirty_flag = _tf;
  }

  bool is_dirty () const noexcept
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
      m_animation {nullptr}
  {}

  AnimSoftValue (V const &_v)
    : SoftValue<V> {_v},
      m_animation {nullptr}
  { }

  ~AnimSoftValue ()
  {
    if (m_animation)
      m_animation->set_finished ();

    m_animation = nullptr;
  }

  void set_animation (AnimationType *_anim)
  {
    m_animation = _anim;
  }

  AnimationType *get_animation () const
  {
    return m_animation;
  }

  AnimationType *m_animation;
};

}
