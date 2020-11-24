#ifndef SORRY_SEA_SIDE
#define SORRY_SEA_SIDE

#include <gst/gst.h>
#include <utility>

namespace charm {

//for use with unique_ptr and glib allocated memory
template<typename T>
struct default_gfree
{
  void operator () (T *_t) const
  {
    g_free (_t);
  }
};

template<typename T>
struct gst_ref_functions;

template<>
struct gst_ref_functions<GstElement>
{
  static GstElement *ref (GstElement *e)
  {
    return reinterpret_cast<GstElement *> (gst_object_ref(e));
  }

  static void unref (GstElement *e)
  {
    gst_object_unref (e);
  }

  static GstElement *ref_sink (GstElement *e)
  {
    return reinterpret_cast<GstElement *> (gst_object_ref_sink(e));
  }
};

template<>
struct gst_ref_functions<GstPad>
{
  static GstPad *ref (GstPad *p)
  {
    return reinterpret_cast<GstPad *> (gst_object_ref(p));
  }

  static void unref (GstPad *p)
  {
    gst_object_unref (p);
  }

  static GstPad *ref_sink (GstPad *p)
  {
    return reinterpret_cast<GstPad *> (gst_object_ref_sink(p));
  }
};

template<>
struct gst_ref_functions<GstObject>
{
  static GstObject *ref (GstObject *o)
  {
    return reinterpret_cast<GstObject *> (gst_object_ref(o));
  }

  static void unref (GstObject *o)
  {
    gst_object_unref (o);
  }

  static GstObject *ref_sink (GstObject *o)
  {
    return reinterpret_cast<GstObject *> (gst_object_ref_sink(o));
  }
};

template<>
struct gst_ref_functions<GstCaps>
{
  static GstCaps *ref (GstCaps *c)
  {
    return gst_caps_ref(c);
  }

  static void unref (GstCaps *c)
  {
    gst_caps_unref (c);
  }
};

template<>
struct gst_ref_functions<GstMessage>
{
  static GstMessage *ref (GstMessage *m)
  {
    return gst_message_ref(m);
  }

  static void unref (GstMessage *m)
  {
    gst_message_unref (m);
  }
};

template<>
struct gst_ref_functions<GstSample>
{
  static GstSample *ref (GstSample *s)
  {
    return gst_sample_ref(s);
  }

  static void unref (GstSample *s)
  {
    gst_sample_unref (s);
  }
};

struct sink_tag {};
struct ref_tag {};

template<typename GstType>
class gst_ptr
{
 public:
  using gst_type = GstType;
  using gst_ptr_type = gst_type *;
  using gst_ref_type = gst_type &;
  using ref_funcs = gst_ref_functions<gst_type>;

  gst_ptr ()
    : object {nullptr}
  { }

  explicit gst_ptr (gst_ptr_type _obj)
    : object {_obj}
  { }

  gst_ptr (gst_ptr_type _obj, ref_tag)
    : object {ref_funcs::ref (_obj)}
  { }

  gst_ptr (gst_ptr_type _obj, sink_tag)
    : object {ref_funcs::ref_sink (_obj)}
  { }

  ~gst_ptr ()
  {
    if (object)
      ref_funcs::unref (object);
    object = nullptr;
  }

  gst_ptr (gst_ptr const &_ptr)
    : object {_ptr ? ref_funcs::ref (_ptr.get ()) : nullptr }
  { }

  gst_ptr (gst_ptr &&_ptr)
    : object {_ptr.get ()}
  {
    _ptr.object = nullptr;
  }

  gst_ptr &operator= (gst_ptr const &_ptr)
  {
    if (this == &_ptr)
      return *this;

    if (object)
      ref_funcs::unref (object);

    object = _ptr ? ref_funcs::ref (_ptr.get ()) : nullptr;
    return *this;
  }

  gst_ptr &operator= (gst_ptr &&_ptr)
  {
    if (this == &_ptr)
      return *this;

    if (object)
      ref_funcs::unref (object);

    object = std::exchange (_ptr.object, nullptr);
    return *this;
  }

  gst_ptr_type get () const noexcept
  {
    return object;
  }

  gst_ptr_type operator-> () const noexcept
  {
    return object;
  }

  operator bool () const noexcept
  {
    return object != nullptr;
  }

  bool operator== (gst_ptr const &_ptr) const noexcept
  {
    return object == _ptr.object;
  }

  bool operator!= (gst_ptr const &_ptr) const noexcept
  {
    return object != _ptr.object;
  }

  gst_ptr ref () const
  {
    return gst_ptr {*this};
  }

  void reset () noexcept
  {
    if (object)
      ref_funcs::unref (object);
    object = nullptr;
  }

  void reset (gst_ptr_type _new_object) noexcept
  {
    if (object)
      ref_funcs::unref (object);
    object = _new_object;
  }

  void reset (gst_ptr_type _new_object, ref_tag) noexcept
  {
    if (object)
      ref_funcs::unref (object);
    object = ref_funcs::ref (_new_object);
  }

  void reset (gst_ptr_type _new_object, sink_tag) noexcept
  {
    if (object)
      ref_funcs::unref (object);
    object = ref_funcs::ref_sink (_new_object);
  }

  gst_ptr_type transfer ()
  {
    return std::exchange (object, nullptr);
  }

  void release ()
  {
    object = nullptr;
  }

 private:
  gst_ptr_type object;
};

}

#endif //SORRY_SEA_SIDE
