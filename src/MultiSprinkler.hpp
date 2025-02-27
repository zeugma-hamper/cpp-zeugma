
//
// (c) treadle & loam, provisioners llc
//

#ifndef WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD
#define WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD


#include <ZeEvent.h>
#include <type_traits>

#include <type_int.hpp>

#include <unordered_map>


namespace zeugma  {


template <typename T>
class Sprinkler final : public ZeEvent::ProtoSprinkler
{
 public:
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
   HOOKUP AppendHandler (U *_receiver, MEMB_FUNC_SIG<U> _func);

  // who owns these? time for ch_ptr?
  void AppendPhage (ch_ptr<OmNihil> const &phage);
  void RemovePhage (ch_ptr<OmNihil> const &phage);
  ch_ptr<OmNihil> ExcisePhage (ch_ptr<OmNihil> const &phage);


 protected:
  SPRNKLR handler_distrib;
  std::vector<ch_weak_ptr<OmNihil>> phages;
};


class MultiSprinkler
{ public:
  MultiSprinkler ();
  ~MultiSprinkler ();

  ZEUGMA_DELETE_COPY  (MultiSprinkler);
  ZEUGMA_DEFAULT_MOVE (MultiSprinkler);

  template <typename T>
   void Spray (T *_event);

  template <typename It>
   void Spray (It _begin, It _end);

  template <typename T>
   ch_ptr<Sprinkler<T>> AssuredSprinklerForEventType ();
  template <typename T>
   ch_ptr<Sprinkler <T>> UnsecuredSprinklerForEventType ();

  template <typename T>
   ZeEvent::ProtoSprinkler::HOOKUP
    AppendHandler (typename Sprinkler<T>::EVT_HANDLER_FUNC &&_func);

  template <typename U, typename T>
   ZeEvent::ProtoSprinkler::HOOKUP
    AppendHandler (U *_receiver,
                   i64 (U::*) (ZeEvent::ProtoSprinkler::HOOKUP const &, T *));

  template <typename T>
   void AppendPhage (ch_ptr<OmNihil> const &phage);
  template <typename T>
   void RemovePhage (ch_ptr<OmNihil> const &phage);
  template <typename T>
   ch_ptr<OmNihil> ExcisePhage (ch_ptr<OmNihil> const &phage);

 protected:
  std::unordered_map <u32, ch_ptr<ZeEvent::ProtoSprinkler>> sprinkler_map;
};


//// Sprinkler methods
template <typename T>
i64 Sprinkler<T>::Spray (ZeEvent *_event)
{ // for debug version
  T *event = dynamic_cast <T *> (_event);
  assert (event);
  handler_distrib (event);
  for (ch_weak_ptr<OmNihil> weak_ph : phages)
    { i64 ret;
      ch_ptr<OmNihil> ph {weak_ph.lock ()};
      if (ph)
        if ((ret = _event->ProfferAsSnackTo (ph . get ()))  >  0)
          return ret;
    }

  return 0;
}

template <typename T>
Sprinkler<T>::operator SPRNKLR & ()
{ return handler_distrib; }

template <typename T>
 ZeEvent::ProtoSprinkler::HOOKUP
  Sprinkler<T>::AppendHandler (EVT_HANDLER_FUNC &&_f)
{
  return handler_distrib
    . connect_extended (std::forward<EVT_HANDLER_FUNC> (_f));
}

template <typename T>
 template <typename U>
ZeEvent::ProtoSprinkler::HOOKUP
 Sprinkler<T>::AppendHandler (U *_receiver,
                              Sprinkler<T>::MEMB_FUNC_SIG<U> _func)
{ return handler_distrib . connect_extended
    ([_receiver, _func] (HOOKUP const &_conn, T *_event)
      { return (_receiver->*_func) (_conn, _event); });
}

template <typename T>
void Sprinkler<T>::AppendPhage (ch_ptr<OmNihil> const &_phage)
{
  phages.push_back (ch_weak_ptr{_phage});
}

template <typename T>
void Sprinkler<T>::RemovePhage (ch_ptr<OmNihil> const &_phage)
{
  ExcisePhage(_phage);
}

template <typename T>
ch_ptr<OmNihil> Sprinkler<T>::ExcisePhage (ch_ptr<OmNihil> const &_phage)
{
  auto it = std::find (phages.begin (), phages.end (), _phage);
  if (it == phages.end ())
    return {};

  phages.erase (it);
  return it->lock();
}

//// [end of Sprinkler methods]


//// MultiSprinkler methods
template <typename T>
void MultiSprinkler::Spray (T *_event)
{
  static_assert (std::is_base_of_v <ZeEvent, T>,
                 "T must be derived from ZeEvent");

  ch_ptr<Sprinkler<T>> sprnk = UnsecuredSprinklerForEventType <T> ();
  if (! sprnk)
    return;

  sprnk -> Spray (_event);
}

template <typename It>
void MultiSprinkler::Spray (It _begin, It _end)
{
  using EvT = std::remove_reference_t <typename It::value_type>;
  static_assert (std::is_base_of_v <ZeEvent, EvT>,
                 "T must be derived from ZeEvent");

  ch_ptr<Sprinkler<EvT>> sprnk = UnsecuredSprinklerForEventType<EvT>();
  if (! sprnk)
    return;

  for (auto cur = _begin; cur != _end; ++_begin)
    sprnk -> Spray (*cur);
}


template <typename T>
ch_ptr<Sprinkler<T>> MultiSprinkler::AssuredSprinklerForEventType ()
{ u32 const idx = ch_index<T>::get ();

  auto const it = sprinkler_map . find (idx);
  if (it  ==  sprinkler_map . end ())
    {
      ch_ptr<Sprinkler<T>> sprnk {new Sprinkler<T>};
      sprinkler_map[idx] = sprnk;
      return sprnk;
    }

  return reinterpret_ch_cast <Sprinkler<T>> (it->second);
}

template <typename T>
ch_ptr<Sprinkler<T>> MultiSprinkler::UnsecuredSprinklerForEventType ()
{ u32 const idx = ch_index<T>::get ();

  auto const it = sprinkler_map . find (idx);
  if (it  ==  sprinkler_map . end ())
    return {};

  return reinterpret_ch_cast <Sprinkler<T>> (it->second);
}

template <typename T>
 ZeEvent::ProtoSprinkler::HOOKUP
  MultiSprinkler::AppendHandler (typename Sprinkler<T>::EVT_HANDLER_FUNC &&_func)
{
  using FUNC_TYPE = typename Sprinkler<T>::EVT_HANDLER_FUNC;
  ch_ptr<Sprinkler <T>> sprnk = AssuredSprinklerForEventType <T> ();
  assert (sprnk);
  return sprnk -> AppendHandler (std::forward<FUNC_TYPE> (_func));
}

template <typename U, typename T>
 ZeEvent::ProtoSprinkler::HOOKUP
  MultiSprinkler::AppendHandler (U *_receiver, i64 (U::* _func)
                                 (ZeEvent::ProtoSprinkler::HOOKUP const &, T *))
{
  ch_ptr<Sprinkler<T>> sprnk = AssuredSprinklerForEventType <T> ();
  assert (sprnk);
  return sprnk -> AppendHandler (_receiver, _func);
}

template <typename T>
void MultiSprinkler::AppendPhage (ch_ptr<OmNihil> const &_phage)
{
  static_assert (std::is_base_of_v <ZeEvent, T>,
                 "T must be derived from ZeEvent");

  ch_ptr<Sprinkler<T>> sprnk = AssuredSprinklerForEventType<T>();
  assert (sprnk);
  sprnk -> AppendPhage (_phage);
}

template <typename T>
void MultiSprinkler::RemovePhage (ch_ptr<OmNihil> const &_phage)
{
  static_assert (std::is_base_of_v <ZeEvent, T>,
                 "T must be derived from ZeEvent");

  ch_ptr<Sprinkler<T>> sprnk = UnsecuredSprinklerForEventType<T>();
  if (sprnk)
    sprnk -> RemovePhage (_phage);
}

template <typename T>
ch_ptr<OmNihil> MultiSprinkler::ExcisePhage (ch_ptr<OmNihil> const &_phage)
{
  static_assert (std::is_base_of_v <ZeEvent, T>,
                 "T must be derived from ZeEvent");

  ch_ptr<Sprinkler<T>> sprnk = UnsecuredSprinklerForEventType<T>();
  if (sprnk)
    return sprnk -> RemovePhage (_phage);

  return {};
}


}  // namespace zeugma and marie antoinette -- just some heads bouncing around


#endif  //WATERING_YOUR_FARMLAND_NOT_JUST_YOUR_YARD
