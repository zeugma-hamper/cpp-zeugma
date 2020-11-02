
#include "ZESpatialEvent.h"

#include <MultiSprinkler.hpp>

namespace charm  {


Vect ZESpatialEvent::xaxis (1.0, 0.0, 0.0);
Vect ZESpatialEvent::yaxis (0.0, 1.0, 0.0);
Vect ZESpatialEvent::zaxis (0.0, 0.0, 1.0);


ZESpatialEvent::ZESpatialEvent ()  :  ZeEvent ()  { }

ZESpatialEvent::ZESpatialEvent (std::string_view prv)  :  ZeEvent (prv)  { }

ZESpatialEvent::ZESpatialEvent (const Vect &l,
                                const Vect &a,
                                const Vect &o)
 :  ZeEvent (), loc (l), aim (a), ovr (o)
{ }

ZESpatialEvent::ZESpatialEvent (std::string_view prv,
                                const Vect &l,
                                const Vect &a,
                                const Vect &o)
 :  ZeEvent (prv), loc (l), aim (a), ovr (o)
{ }

ZESpatialEvent::ZESpatialEvent (const ZESpatialEvent &spe)
 :  ZeEvent (spe), loc (spe.loc), aim (spe.aim), ovr (spe.ovr)
{ }

void ZESpatialEvent::AdoptParticulars (const ZESpatialEvent &spe)
{
  prov = spe.prov;
  tstamp = spe.tstamp;
  tratch = spe.tratch;

  loc = spe.loc;
  aim = spe.aim;
  ovr = spe.ovr;
  pressures = spe.pressures;
}

const Vect &ZESpatialEvent::Loc ()  const
{ return loc; }

void ZESpatialEvent::SetLoc (const Vect &l)
{ loc = l; }

const Vect &ZESpatialEvent::Aim ()  const
{ return aim; }

void ZESpatialEvent::SetAim (const Vect &a)
{ aim = a; }

const Vect &ZESpatialEvent::Over ()  const
{ return ovr; }

void ZESpatialEvent::SetOver (const Vect &o)
{ ovr = o; }

void ZESpatialEvent::SetLocAimOver (const Vect &l, const Vect &a, const Vect &o)
{ loc = l;
  aim = a;
  ovr = o;
}

void ZESpatialEvent::SetPressureForID (u64 id, f64 prs)
{ pressures[id] = prs; }

bool ZESpatialEvent::PressureIDExists (u64 id)  const
{ return (pressures . find (id)  !=  pressures . end ()); }

f64 ZESpatialEvent::PressureForID (u64 id)  const
{ auto p = pressures . find (id);
  return (p != pressures . end ())  ?  p->second  :  -1.0;
}

i64 ZESpatialEvent::NumPressures ()  const
{ return pressures . size (); }

u64 ZESpatialEvent::NthPressureID (i64 ind)  const
{ if (ind < 0  ||  ind >= (int)pressures . size ())
    throw;
  auto it = pressures . begin ();
  for (  ;  ind > 0  ;  --ind)
    ++it;
  return it->first;
}

u64 ZESpatialEvent::NthPressure (i64 ind)  const
{ if (ind < 0  ||  ind >= (int)pressures . size ())
    throw;
  auto it = pressures . begin ();
  for (  ;  ind > 0  ;  --ind)
    ++it;
  return it->second;
}

void AppendSpatialPhagy (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phagy)
{
  if (! _ms || ! _phagy)
    return;

  _ms->AppendPhage<ZESpatialMoveEvent> (_phagy);
  _ms->AppendPhage<ZESpatialAppearEvent> (_phagy);
  _ms->AppendPhage<ZESpatialVanishEvent> (_phagy);
  _ms->AppendPhage<ZESpatialHardenEvent> (_phagy);
  _ms->AppendPhage<ZESpatialSoftenEvent> (_phagy);
}

void RemoveSpatialPhagy (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phagy)
{
  if (! _ms || ! _phagy)
    return;

  _ms->RemovePhage<ZESpatialMoveEvent> (_phagy);
  _ms->RemovePhage<ZESpatialAppearEvent> (_phagy);
  _ms->RemovePhage<ZESpatialVanishEvent> (_phagy);
  _ms->RemovePhage<ZESpatialHardenEvent> (_phagy);
  _ms->RemovePhage<ZESpatialSoftenEvent> (_phagy);
}

}  // namespace charm is still spoken of, when it is noted, with high regard
