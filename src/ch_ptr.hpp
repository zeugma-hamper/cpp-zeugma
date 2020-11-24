#ifndef YOUR_FINGER_IS_IN_MY_EYE
#define YOUR_FINGER_IS_IN_MY_EYE

#include "base_types.hpp"

#include <atomic>
#include <type_traits>

namespace charm
{

class ReferenceCounter
{
 public:
  ReferenceCounter ()
    : m_strong_count {1u},
      m_weak_count {0u}
  { }

  inline void add_ref ()
  {
    ++m_strong_count;
  }

  inline void add_weak_ref ()
  {
    ++m_weak_count;
  }

  inline bool dec_ref ()
  {
    --m_strong_count;
    return m_strong_count == 0 && m_weak_count == 0;
  }

  inline bool dec_weak_ref ()
  {
    --m_weak_count;
    return m_strong_count == 0 && m_weak_count == 0;
  }

  inline bool expired () const noexcept
  {
    return m_strong_count == 0;
  }

 private:
  u32 m_strong_count;
  u32 m_weak_count;
};

//not entirely confidant about multi-threaded version
class MTReferenceCounter
{
 public:
  MTReferenceCounter ()
    : m_counts {u64(1) << 32}
  { }

  inline void add_ref ()
  {
    m_counts.fetch_add (u64(1) << 32);
  }

  inline void add_weak_ref ()
  {
    m_counts.fetch_add (1u);
  }

  inline bool dec_ref ()
  {
    //fetch_sub returns value prior to modification
    return m_counts.fetch_sub (u64(1) << 32) == u64(1) << 32;
  }

  inline bool dec_weak_ref ()
  {
    //fetch_sub returns value prior to modification
    return m_counts.fetch_sub (1) == u64(1);
  }

  inline bool expired () const noexcept
  {
    return (m_counts.load () & (u64 (0xFFFFFFFF) << 32)) == u64(0);
  }

 private:
  //u64 = 0xYYYYYYYYXXXXXXXX, where Y is strong ref, X is weak ref
  std::atomic<u64> m_counts;
};

template<typename T>
class ch_ptr;

template<typename T, typename ReferenceOps = ReferenceCounter>
class CharmBase
{
 public:
  template<typename U>
  friend class ch_ptr;
  template<typename U>
  friend class ch_weak_ptr;

  using Type = T;
  using RefOps = ReferenceOps;

  CharmBase () = default;

  CharmBase (CharmBase const &) = delete;
  CharmBase &operator= (CharmBase const &) = delete;

  ch_ptr<T> ch_from_this ();

  void add_reference ()
  {
    m_counter.add_ref();
  }

  void add_weak_reference ()
  {
    m_counter.add_weak_ref();
  }

  void remove_reference ()
  {
    if (m_counter.dec_ref())
      delete static_cast<T *> (this);
  }

  void remove_weak_reference ()
  {
    if (m_counter.dec_weak_ref())
      delete static_cast<T *> (this);
  }

protected:
  RefOps m_counter;
};

template<typename T>
class ch_ptr
{
 public:
  template<typename U>
  friend class ch_ptr;

  ch_ptr ()
    : m_pointer {nullptr}
  { }

  explicit ch_ptr (T *_t)
    : m_pointer {_t}
  { }

  template<typename U>
  explicit ch_ptr (U *_t)
    : m_pointer {static_cast<T *> (_t)}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
  }

  ~ch_ptr ()
  {
    reset ();
  }

  ch_ptr (ch_ptr const &_t)
    : m_pointer {_t.m_pointer}
  {
    if (m_pointer)
      m_pointer->add_reference ();
  }

  ch_ptr (ch_ptr &&_t) noexcept
    : m_pointer {_t.m_pointer}
  {
    _t.m_pointer = nullptr;
  }

  template<typename U>
  ch_ptr (ch_ptr<U> const &_t)
    : m_pointer {static_cast<T *> (_t.get ())}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    if (m_pointer)
      m_pointer->add_reference ();
  }

  template<typename U>
  ch_ptr (ch_ptr<U> &&_t) noexcept
    : m_pointer {static_cast<T *> (_t.get ())}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    _t.m_pointer = nullptr;
  }

  ch_ptr &operator=(ch_ptr const &_t)
  {
    if (this == &_t)
      return *this;

    reset ();

    m_pointer = _t.m_pointer;
    if (m_pointer)
      m_pointer->add_reference ();

    return *this;
  }

  ch_ptr &operator=(ch_ptr &&_t) noexcept
  {
    reset ();

    m_pointer = _t.m_pointer;
    _t.m_pointer = nullptr;

    return *this;
  }

  template<typename U>
  ch_ptr &operator=(ch_ptr<U> const &_t)
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    reset ();

    m_pointer = static_cast<T *> (_t.m_pointer);
    if (m_pointer)
      m_pointer->add_reference ();

    return *this;
  }

  template<typename U>
  ch_ptr &operator=(ch_ptr<U> &&_t) noexcept
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    reset ();

    m_pointer = static_cast<T *> (_t.m_pointer);
    _t.m_pointer = nullptr;

    return *this;
  }

  T *get () const noexcept
  {
    return m_pointer;
  }

  T &operator* () const noexcept
  {
    return *get ();
  }

  T *operator-> () const noexcept
  {
    return get ();
  }

  operator bool () const noexcept
  {
    return m_pointer != nullptr;
  }

  void reset ()
  {
    if (m_pointer)
      m_pointer->remove_reference ();

    m_pointer = nullptr;
  }

 private:
  T *m_pointer;
};

template<typename T, typename... Args>
ch_ptr<T> make_ch (Args &&..._args)
{
  return ch_ptr<T> (new T (std::forward<Args> (_args)...));
}

template<typename T>
class ch_weak_ptr
{
 public:

  ch_weak_ptr () noexcept
    : m_pointer {nullptr}
  { }

  ch_weak_ptr (ch_weak_ptr const &_t)
    : m_pointer {_t.get ()}
  {
    if (m_pointer)
      m_pointer->add_weak_reference ();
  }

  // move version accesses pointer directly, copy uses `get`
  // so copy of expired weak pointer doesn't copy expired pointer
  // but move of expired weak pointer does copy pointer.
  // i'm not entirely sure about point.
  ch_weak_ptr (ch_weak_ptr &&_t) noexcept
    : m_pointer {_t.m_pointer}
  {
    _t.m_pointer = nullptr;
  }

  template<typename U>
  ch_weak_ptr (ch_weak_ptr<U> const &_t)
    : m_pointer {static_cast<T *> (_t.get ())}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");

    if (m_pointer)
      m_pointer->add_weak_reference ();
  }

  template<typename U>
  ch_weak_ptr (ch_weak_ptr<U> &&_t) noexcept
    : m_pointer {static_cast<T *> (_t.m_pointer)}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    _t.m_pointer = nullptr;
  }

  explicit ch_weak_ptr (ch_ptr<T> const &_ipt)
    : m_pointer {_ipt.get ()}
  {
    if (m_pointer)
      m_pointer->add_weak_reference ();
  }

  template<typename U>
  explicit ch_weak_ptr (ch_ptr<U> const &_ipt)
    : m_pointer {static_cast<T *> (_ipt.get ())}
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    if (m_pointer)
      m_pointer->add_weak_reference ();
  }

  ch_weak_ptr &operator= (ch_ptr<T> const &_ipt)
  {
    reset ();
    m_pointer = _ipt.get ();
    if (m_pointer)
      m_pointer->add_weak_reference ();

    return *this;
  }

  template<typename U>
  ch_weak_ptr &operator= (ch_ptr<U> const &_ipt)
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    reset ();
    m_pointer = static_cast<T *> (_ipt.get ());
    if (m_pointer)
      m_pointer->add_weak_reference ();

    return *this;
  }

  ~ch_weak_ptr ()
  {
    reset ();
  }

  ch_weak_ptr &operator=(ch_weak_ptr const &_t)
  {
    if (this == &_t)
      return *this;

    reset ();

    m_pointer = _t.get ();
    if (m_pointer)
      m_pointer->add_weak_reference ();

    return *this;
  }

  ch_weak_ptr &operator=(ch_weak_ptr &&_t)
  {
    reset ();

    m_pointer = _t.m_pointer;
    _t.m_pointer = nullptr;

    return *this;
  }

  template<typename U>
  ch_weak_ptr &operator=(ch_weak_ptr<U> const &_t)
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");

    reset ();

    m_pointer = static_cast<T *> (_t.get ());
    if (m_pointer)
      m_pointer->add_weak_reference ();

    return *this;
  }

  template<typename U>
  ch_weak_ptr &operator=(ch_weak_ptr<U> &&_t)
  {
    static_assert (std::is_base_of_v<T, U>, "Argument must be derived class");
    reset ();

    m_pointer = static_cast<T *> (_t.m_pointer);
    _t.m_pointer = nullptr;

    return *this;
  }

  operator bool () const noexcept
  {
    return !expired ();
  }

  void reset ()
  {
    if (m_pointer)
      m_pointer->remove_weak_reference ();

    m_pointer = nullptr;
  }

  bool expired () const noexcept
  {
    return !m_pointer || m_pointer->m_counter.expired ();
  }

  // not for usage, just for equality check
  // to get a usable pointer, call lock
  T *get () const noexcept
  {
    if (expired ())
      return nullptr;

    return m_pointer;
  }

  T *get_unchecked () const noexcept
  {
    return m_pointer;
  }

  ch_ptr<T> ref () const noexcept
  {
    if (! expired ())
      {
        m_pointer->add_reference ();
        return ch_ptr<T> {m_pointer};
      }

    return {};
  }

  ch_ptr<T> lock () const noexcept
  {
    return ref ();
  }

 private:
  T *m_pointer;
};

template<typename T, typename RO>
ch_ptr<T> CharmBase<T, RO>::ch_from_this ()
{
  add_reference();
  return ch_ptr<T>{static_cast<T *> (this)};
}


template<typename T, typename U>
ch_ptr<T> dynamic_ch_cast (ch_ptr<U> const &_ptr)
{
  T *t = dynamic_cast<T *> (_ptr.get ());
  if (t)
    t->add_reference ();

  return ch_ptr<T>{t};
}

template<typename T, typename U>
ch_ptr<T> static_ch_cast (ch_ptr<U> const &_ptr)
{
  T *t = static_cast<T *> (_ptr.get ());
  if (t)
    t->add_reference ();

  return ch_ptr<T>{t};
}

template<typename T, typename U>
ch_ptr<T> const_ch_cast (ch_ptr<U> const &_ptr)
{
  T *t = const_cast<T *> (_ptr.get ());
  if (t)
    t->add_reference ();

  return ch_ptr<T>{t};
}

template<typename T, typename U>
ch_ptr<T> reinterpret_ch_cast (ch_ptr<U> const &_ptr)
{
  T *t = reinterpret_cast<T *> (_ptr.get ());
  if (t)
    t->add_reference ();

  return ch_ptr<T>{t};
}

//operator== and operator!= for ch_ptr on the left
template<typename T, typename U>
inline bool operator== (ch_ptr<T> const &_left, ch_ptr<U> const &_right)
{
  return _left.get () == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (ch_ptr<T> const &_left, ch_ptr<U> const &_right)
{
  return !(_left == _right);
}

template<typename T, typename U>
inline bool operator== (ch_ptr<T> const &_left, ch_weak_ptr<U> const &_right)
{
  return _left.get () == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (ch_ptr<T> const &_left, ch_weak_ptr<U> const &_right)
{
  return !(_left == _right);
}

template<typename T, typename U>
inline bool operator== (ch_ptr<T> const &_left, U *_right)
{
  return _left.get () == _right;
}

template<typename T, typename U>
inline bool operator!= (ch_ptr<T> const &_left, U *_right)
{
  return !(_left.get () == _right);
}

//operator== and operator!= for ch_weak_ptr on the left
template<typename T, typename U>
inline bool operator== (ch_weak_ptr<T> const &_left, ch_weak_ptr<U> const &_right)
{
  return _left.get () == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (ch_weak_ptr<T> const &_left, ch_weak_ptr<U> const &_right)
{
  return !(_left == _right);
}

template<typename T, typename U>
inline bool operator== (ch_weak_ptr<T> const &_left, ch_ptr<U> const &_right)
{
  return _left.get () == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (ch_weak_ptr<T> const &_left, ch_ptr<U> const &_right)
{
  return !(_left == _right);
}

template<typename T, typename U>
inline bool operator== (ch_weak_ptr<T> const &_left, U *_right)
{
  return _left.get () == _right;
}

template<typename T, typename U>
inline bool operator!= (ch_weak_ptr<T> const &_left, U *_right)
{
  return !(_left.get () == _right);
}

//operator== and operator!= for bare pointer on the left
template<typename T, typename U>
inline bool operator== (T *_left, ch_weak_ptr<U> const &_right)
{
  return _left == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (T *_left, ch_weak_ptr<U> const &_right)
{
  return !(_left == _right);
}

template<typename T, typename U>
inline bool operator== (T *_left, ch_ptr<U> const &_right)
{
  return _left == _right.get ();
}

template<typename T, typename U>
inline bool operator!= (T *_left, ch_ptr<U> const &_right)
{
  return !(_left == _right);
}

}
#endif //YOUR_FINGER_IS_IN_MY_EYE
