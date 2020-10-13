
#ifndef WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD
#define WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD


#include <ZeEvent.h>
#include <type_traits>

#include <type_int.hpp>

#include <unordered_map>


namespace charm
{


template <typename T>
class Sprinkler final : public ZeEvent::ProtoSprinkler
{ public:
  static_assert (std::is_base_of_v<ZeEvent, T>,
                 "T must be derived from ZeEvent");

  using SPRNKLR = boost::signals2::signal <i64 (T *)>;
  using EVT_HANDLER_FUNC = typename SPRNKLR::extended_slot_function_type;

  template <typename U>
  using MEMB_FUNC_SIG = i64 (U::*) (HOOKUP const &, T *);

  operator SPRNKLR & ();

  i64 Spray (ZeEvent *_event) override final;

  HOOKUP AppendHandler (EVT_HANDLER_FUNC &&_f);

  template <typename U>
  HOOKUP AppendHandler (U *_receiver, MEMB_FUNC_SIG<U> &&_func);

 protected:
  SPRNKLR sp;
};


class MultiSprinkler
{ public:
  MultiSprinkler ();
  ~MultiSprinkler ();

  CHARM_DELETE_COPY (MultiSprinkler);
  CHARM_DEFAULT_MOVE (MultiSprinkler);

  template <typename T>
  void Spray (T *_event);

  template <typename T>
  Sprinkler <T> *SprinklerForEventType ();

  template<typename T>
  ZeEvent::ProtoSprinkler::HOOKUP AppendHandler (typename Sprinkler<T>::EVT_HANDLER_FUNC &&_func);

  template<typename T, typename U>
  ZeEvent::ProtoSprinkler::HOOKUP AppendHandler (U *_receiver,
                                                 typename Sprinkler<T>::template MEM_FUNC_SIG<U> &&_func);

 protected:
  std::unordered_map<u32, ZeEvent::ProtoSprinkler *> sprinkler_map;
};


//// Sprinkler methods
template<typename T>
i64 Sprinkler<T>::Spray (ZeEvent *_event)
{ // for debug version
  T *event = dynamic_cast <T *> (_event);
  assert (event);
  sp (event);
  return 0;
}

template<typename T>
Sprinkler<T>::operator SPRNKLR & ()
{ return sp; }

template <typename T>
 ZeEvent::ProtoSprinkler::HOOKUP
Sprinkler<T>::AppendHandler (EVT_HANDLER_FUNC &&_f)
{
  return sp.connect_extended (std::forward (_f));
}

template<typename T>
 template<typename U>
ZeEvent::ProtoSprinkler::HOOKUP
 Sprinkler<T>::AppendHandler (U *_receiver,
                              Sprinkler<T>::MEMB_FUNC_SIG<U> &&_func)
{ return sp.connect_extended
    ([_receiver, _func] (HOOKUP const &_conn, T *_event)
      { return (_receiver->*_func) (_conn, _event); });
}


//// [end of Sprinkler methods]


//// MultiSprinkler methods
template<typename T>
void MultiSprinkler::Spray (T *_event)
{ static_assert (std::is_base_of_v <ZeEvent, T>,
                 "T must be derived from ZeEvent");

  auto it = sprinkler_map . find (index<T>::get ());
  if (it  ==  sprinkler_map . end ())
    return;

  (it->second) -> Spray (_event);
}

template <typename T>
Sprinkler<T> *MultiSprinkler::SprinklerForEventType ()
{ u32 const idx = index<T>::get ();

  auto const it = sprinkler_map . find (idx);
  if (it  ==  sprinkler_map . end ())
    { auto sp = new Sprinkler<T>;
      sprinkler_map[idx] = sp;
      return sp;
    }

  return (Sprinkler<T> *)(it->second);
}

template <typename T>
 ZeEvent::ProtoSprinkler::HOOKUP
  MultiSprinkler::AppendHandler (typename Sprinkler<T>::EVT_HANDLER_FUNC &&_func)
{ Sprinkler <T> *sp = SprinklerForEventType <T> ();
  assert (sp);
  return sp->AppendHandler (std::forward (_func));
}

template<typename T, typename U>
ZeEvent::ProtoSprinkler::HOOKUP
MultiSprinkler::AppendHandler (U *_receiver,
                               typename Sprinkler<T>::template MEM_FUNC_SIG<U> &&_func)
{
  Sprinkler<T> &sp = SprinklerForEventType<T>();
  assert (sp);
  return sp.AppendHandler(_receiver, std::forward (_func));
}


}  // namespace charm and marie antoinette -- just some heads bouncing around


#endif //WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD
