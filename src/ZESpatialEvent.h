
//
// (c) treadle & loam, provisioners llc
//

#ifndef ZE_SPATIAL_EVENT_WITH_THREE_NITRO_BURNING_DIMENSIONS_AT_ZE_CENTRUM
#define ZE_SPATIAL_EVENT_WITH_THREE_NITRO_BURNING_DIMENSIONS_AT_ZE_CENTRUM

#include "ZeEvent.h"

#include "Vect.h"

#include <map>


namespace zeugma  {


class ZESpatialEvent  :  public ZeEvent
{ ZE_EVT_TUMESCE (ZESpatial, Ze);

 public:
  Vect loc, aim, ovr;
  std::map <u64, f64> pressures;

  static Vect xaxis, yaxis, zaxis;

  ZESpatialEvent ();
  ZESpatialEvent (std::string_view prv);
  ZESpatialEvent (const Vect &l,
                  const Vect &a = -zaxis,
                  const Vect &o = xaxis);
  ZESpatialEvent (std::string_view prv,
                  const Vect &l,
                  const Vect &a = -zaxis,
                  const Vect &o = xaxis);

  ZESpatialEvent (const ZESpatialEvent &spe);

  void AdoptParticulars (const ZESpatialEvent &spe);

  const Vect &Loc ()  const;
  void SetLoc (const Vect &l);

  const Vect &Aim ()  const;
  void SetAim (const Vect &a);

  const Vect &Over ()  const;
  void SetOver (const Vect &o);
  void SetLocAimOver (const Vect &l, const Vect &a, const Vect &o);

  void SetPressureForID (u64 id, f64 prs);
  bool PressureIDExists (u64 id)  const;
  f64  PressureForID (u64 id)  const;
  i64  NumPressures ()  const;
  u64  NthPressureID (i64 ind)  const;
  f64  NthPressure (i64 ind)  const;
};


class ZESpatialMoveEvent  :  public ZESpatialEvent
{ ZE_EVT_TUMESCE (ZESpatialMove, ZESpatial);

  ZESpatialMoveEvent ()  :  ZESpatialEvent ()  { }
  ZESpatialMoveEvent (std::string_view prv)  :  ZESpatialEvent (prv)  { }
  ZESpatialMoveEvent (std::string_view prv,
                      const Vect &l,
                      const Vect &a = -zaxis,
                      const Vect &o = xaxis)
     :  ZESpatialEvent (prv, l, a, o)
    { }

  // this following -- and all similar below -- is not (are not) copy
  // constructors; rather, they're sort of 'promotion constructors':
  // view with a penetrating gaze!
  ZESpatialMoveEvent (const ZESpatialEvent &spe)
     :  ZESpatialEvent (spe)
    { }
};


class ZESpatialAppearEvent  :  public ZESpatialEvent
{ ZE_EVT_TUMESCE (ZESpatialAppear, ZESpatial);

  ZESpatialAppearEvent ()  :  ZESpatialEvent ()  { }
  ZESpatialAppearEvent (std::string_view prv)  :  ZESpatialEvent (prv)  { }
  ZESpatialAppearEvent (std::string_view prv,
                        const Vect &l,
                        const Vect &a = -zaxis,
                        const Vect &o = xaxis)
     :  ZESpatialEvent (prv, l, a, o)
    { }

  ZESpatialAppearEvent (const ZESpatialEvent &spe)
     :  ZESpatialEvent (spe)
    { }
};


class ZESpatialVanishEvent  :  public ZESpatialEvent
{ ZE_EVT_TUMESCE (ZESpatialVanish, ZESpatial);

  ZESpatialVanishEvent ()  :  ZESpatialEvent ()  { }
  ZESpatialVanishEvent (std::string_view prv)  :  ZESpatialEvent (prv)  { }
  ZESpatialVanishEvent (std::string_view prv,
                        const Vect &l,
                        const Vect &a = -zaxis,
                        const Vect &o = xaxis)
     :  ZESpatialEvent (prv, l, a, o)
    { }

  ZESpatialVanishEvent (const ZESpatialEvent &spe)
     :  ZESpatialEvent (spe)
    { }
};


class ZESpatialPressureEvent  :  public ZESpatialEvent
{ ZE_EVT_TUMESCE (ZESpatialPressure, ZESpatial);

  u64 which_pressor;
  f64 updated_value;

  ZESpatialPressureEvent ()  :  ZESpatialEvent ()  { }
  ZESpatialPressureEvent (std::string_view prv)  :  ZESpatialEvent (prv)  { }
  ZESpatialPressureEvent (std::string_view prv, u64 prs_id, f64 prs_val)
     :  ZESpatialEvent (prv), which_pressor (prs_id), updated_value (prs_val)
    { }

  ZESpatialPressureEvent (const ZESpatialEvent &spe, u64 prs_id, f64 prs_val)
     :  ZESpatialEvent (spe), which_pressor (prs_id), updated_value (prs_val)
    { }

  u64 WhichPressor ()  const
    { return which_pressor; }
  f64 UpdatedValue ()  const
    { return updated_value; }
};


class ZESpatialHardenEvent  :  public ZESpatialPressureEvent
{ ZE_EVT_TUMESCE (ZESpatialHarden, ZESpatialPressure);

  ZESpatialHardenEvent ()  :  ZESpatialPressureEvent ()  { }
  ZESpatialHardenEvent (std::string_view prv)
     :  ZESpatialPressureEvent (prv)
    { }
  ZESpatialHardenEvent (std::string_view prv, u64 prs_id, f64 prs_val = 1.0)
     :  ZESpatialPressureEvent (prv, prs_id, prs_val)
    { }

  ZESpatialHardenEvent (const ZESpatialEvent &spe,
                        u64 prs_id, f64 prs_val = 1.0)
     :  ZESpatialPressureEvent (spe, prs_id, prs_val)
    { }
};


class ZESpatialSoftenEvent  :  public ZESpatialPressureEvent
{ ZE_EVT_TUMESCE (ZESpatialSoften, ZESpatialPressure);

  ZESpatialSoftenEvent ()  :  ZESpatialPressureEvent ()  { }
  ZESpatialSoftenEvent (std::string_view prv)
     :  ZESpatialPressureEvent (prv)
    { }
  ZESpatialSoftenEvent (std::string_view prv, u64 prs_id, f64 prs_val = 0.0)
     :  ZESpatialPressureEvent (prv, prs_id, prs_val)
    { }

  ZESpatialSoftenEvent (const ZESpatialEvent &spe,
                        u64 prs_id, f64 prs_val = 0.0)
     :  ZESpatialPressureEvent (spe, prs_id, prs_val)
    { }
};



class ZESpatialPhagy
  :  //public ZESpatialEvent::ZESpatialPhage,
     public ZESpatialMoveEvent::ZESpatialMovePhage,
     public ZESpatialAppearEvent::ZESpatialAppearPhage,
     public ZESpatialVanishEvent::ZESpatialVanishPhage,
     //public ZESpatialPressureEvent::ZESpatialPressurePhage,
     public ZESpatialHardenEvent::ZESpatialHardenPhage,
     public ZESpatialSoftenEvent::ZESpatialSoftenPhage
{ };


class MultiSprinkler;

void AppendSpatialPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage);
void RemoveSpatialPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage);


template <typename T>
void AppendSpatialPhage (MultiSprinkler *_ms, ch_ptr<T> const &_phage)
{
  AppendSpatialPhage (_ms, static_ch_cast <OmNihil> (_phage));
}

template <typename T>
void RemoveSpatialPhage (MultiSprinkler *_ms, ch_ptr<T> const &_phage)
{
  RemoveSpatialPhage (_ms, static_ch_cast <OmNihil> (_phage));
}


}  // snuffed it, did namespace zeugma, o me droogs...


#endif  //ZE_SPATIAL_EVENT_WITH_THREE_NITRO_BURNING_DIMENSIONS_AT_ZE_CENTRUM
