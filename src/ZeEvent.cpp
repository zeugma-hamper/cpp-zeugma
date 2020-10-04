
#include <ZeEvent.h>

#include "MotherTime.h"

#include <type_int.hpp>


namespace charm
{


ZeEvent::ZeEvent ()  :  Zeubject (),
                        //tstamp (MotherTime::AbsoluteTime ()),
                        tstamp (-1.0),
                        tratch (-1), fore_evt (NULL)
{ }

ZeEvent::ZeEvent (std::string_view p,
         f64 ts,
         // f64 ts = MotherTime::AbsoluteTime (),
         i64 tr)
 :  Zeubject (), prov (p), tstamp (ts), tratch (tr), fore_evt (NULL)
{ }

ZeEvent::ZeEvent (f64 ts, i64 tr)
 :  Zeubject (), tstamp (ts), tratch (tr), fore_evt (NULL)
{ }

ZeEvent::ZeEvent (i64 tr)
 :  Zeubject (),
    tstamp (-1.0),
    // tstamp (MotherTime::AbsoluteTime ()),
    tratch (tr), fore_evt (NULL)
{ }

ZeEvent::ZeEvent (const ZeEvent &zee)
 :  Zeubject (),
    prov (zee.prov),
    tstamp (zee.tstamp), tratch (zee.tratch),
    fore_evt (NULL)  // or maybe "&zee"?
{ }


const std::string &ZeEvent::Provenance ()  const
{ return prov; }

void ZeEvent::SetProvenance (std::string_view p)
{ prov = p; }


f64 ZeEvent::Timestamp ()  const
{ return tstamp; }

void ZeEvent::SetTimestamp (f64 ts)
{ tstamp = ts; }

void ZeEvent::CurrentifyTimestamp ()
{ tstamp = MotherTime::AbsoluteTime (); }


i64 ZeEvent::TimeRatchet ()  const
{ return tratch; }

void ZeEvent::SetTimeRatchet (i64 tr)
{ tratch = tr; }


ZeEvent *ZeEvent::ForebearEvent ()  const
{ return fore_evt; }

void ZeEvent::SetForebearEvent (ZeEvent *fe)
{ fore_evt = fe; }


const std::string &EventIlk ()
{ static std::string ev_ilk ("ZeEvent");
  return ev_ilk;
}

const std::string &EventSuperIlk ()
{ static std::string ev_silk ("");
  return ev_silk;
}

u32 ZeEvent::EventTypeIndex () const
{ return index<ZeEvent>::get (); }

u32 ZeEvent::EventSuperTypeIndex () const
{ return u32 (-1); }


}  // namespace charm off to see what dante and virgil saw
