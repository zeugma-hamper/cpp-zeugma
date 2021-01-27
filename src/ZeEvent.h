
#ifndef ZE_EVENT_IS_ALL_ZE_RAGE
#define ZE_EVENT_IS_ALL_ZE_RAGE


//#include "MotherTime.h"
#include "Zeubject.h"

#include <class_utils.hpp>
#include <type_int.hpp>

#include <string>
#include <string_view>
#include <map>
#include <vector>

#include <boost/signals2/signal.hpp>


namespace charm
{


class ZeEvent  :  public Zeubject
{ public:
  std::string prov;
  f64 tstamp;
  i64 tratch;
  ZeEvent *fore_evt;

  ZeEvent ();
  ZeEvent (std::string_view p,
           f64 ts = -1.0,
           i64 tr = -1);
  ZeEvent (f64 ts, i64 tr = -1);
  ZeEvent (i64 tr);

  ZeEvent (const ZeEvent &zee);


  const std::string &Provenance ()  const;
  void SetProvenance (std::string_view p);

  f64  Timestamp ()  const;
  void SetTimestamp (f64 ts);
  void CurrentifyTimestamp ();

  i64  TimeRatchet ()  const;
  void SetTimeRatchet (i64 tr);

  ZeEvent *ForebearEvent ()  const;
  void     SetForebearEvent (ZeEvent *fe);

  virtual const std::string &EventIlk ()  const;
  virtual const std::string &EventSuperIlk ()  const;

  virtual u32 EventTypeIndex ()  const;
  virtual u32 EventSuperTypeIndex ()  const;

  class ProtoSprinkler  :  public Zeubject
    { public:
      typedef boost::signals2::connection HOOKUP;
      // implementations send the event along to targets
      virtual ~ProtoSprinkler () { }
      virtual i64 Spray (ZeEvent *)
        { return -1; }
    };

  class ZePhage
    { public:
      virtual ~ZePhage () { }
      // implementations receive the event and do their bidness
      virtual i64 Ze (ZeEvent *)
        { return -1; }
      virtual bool PassTheBuckUpPhageHierarchy ()
        { return false; }
    };

  virtual i64 ProfferAsSnackTo (OmNihil *om)
    { if (ZePhage *ph = dynamic_cast <ZePhage *> (om))
        return ph -> Ze (this);
      return -1;
    }
};


class EventSprinklerGroup  :  public Zeubject
{ public:
  using SprBunch = std::vector <ZeEvent::ProtoSprinkler *>;
  std::map <std::string, SprBunch> sprnks_by_evt_ilk;
  void AppendSprinkler (const std::string &ev_ilk, ZeEvent::ProtoSprinkler *s)
    { SprBunch &sbun = sprnks_by_evt_ilk[ev_ilk];
      if (s)
        sbun . push_back (s);
    }
  SprBunch &SprinklerBunchForEventIlk (const std::string &ilk)
    { return sprnks_by_evt_ilk[ilk]; }
  i64 MetaSpray (ZeEvent *e)
    { if (! e)
        return -1;
      SprBunch &sbun = sprnks_by_evt_ilk[e -> EventIlk ()];
      for (ZeEvent::ProtoSprinkler *s  :  sbun)
        s -> Spray (e);
      return 0;
    }
};



#define ZE_EVT_TUMESCE(EV_ILK, EV_SUPERILK)                             \
 public:                                                                \
  class EV_ILK##Phage  :  public EV_SUPERILK##Phage                     \
    { public:                                                           \
      virtual i64 EV_ILK (EV_ILK##Event *e)                             \
        { if (PassTheBuckUpPhageHierarchy ())                           \
            return EV_SUPERILK (e);                                     \
          return -1;                                                    \
        }                                                               \
    };                                                                  \
  const std::string &EventIlk ()  const override                        \
    { static std::string ev_ilk (#EV_ILK);                              \
      return ev_ilk;                                                    \
    }                                                                   \
  const std::string &EventSuperIlk ()  const override                   \
    { static std::string ev_silk (#EV_SUPERILK);                        \
      return ev_silk;                                                   \
    }                                                                   \
  u32 EventTypeIndex () const override                                  \
    { return ch_index<EV_ILK ## Event>::get (); }                       \
  u32 EventSuperTypeIndex () const override                             \
    { return ch_index<EV_SUPERILK ## Event>::get ();}                   \
  i64 ProfferAsSnackTo (OmNihil *om)  override                          \
    { if (EV_ILK##Phage *ph = dynamic_cast <EV_ILK##Phage *> (om))      \
        { i64 sult = ph -> EV_ILK (this);                               \
          if (sult >= -1)                                               \
            return sult;                                                \
        }                                                               \
      return -2;                                                        \
    }                                                                   \
  class Sprinkler  :  public ZeEvent::ProtoSprinkler                    \
    { public:                                                           \
      typedef boost::signals2::signal <i64 (EV_ILK##Event *)> SPRNKLR;  \
      typedef SPRNKLR::extended_slot_function_type EVT_HANDLER_FUNC;    \
      SPRNKLR s;                                                        \
      operator SPRNKLR & ()  { return s; }                              \
      HOOKUP AppendHandler (EVT_HANDLER_FUNC f)                         \
        { return s . connect_extended (f); }                            \
      i64 Spray (ZeEvent *e)  override                                  \
        { s (dynamic_cast <EV_ILK##Event *> (e));  return 0; }          \
    };


}  // namespace charm down there, gobbling all the pomegrantae seeds; no hermes


#endif //ZE_EVENT_IS_ALL_ZE_RAGE
