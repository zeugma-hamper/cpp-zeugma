
#ifndef RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS
#define RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS


#include "Zeubject.h"
#include "ZESpatialEvent.h"

#include "Matrix44.h"

#include "MultiSprinkler.hpp"

#include <lo/lo_cpp.h>


namespace charm  {


class RawEventParser  :  public Zeubject
{ public:
  EventSprinklerGroup *esg;
  MultiSprinkler *mulspri;
  RawEventParser ()  :  Zeubject (), esg (NULL)
    { }
  EventSprinklerGroup *SprinklerGroup ()
    { return esg; }
  void InstallSprinklerGroup (EventSprinklerGroup *sprigrou)
    { esg = sprigrou; }
  MultiSprinkler *SoleMultiSprinkler ()
    { return mulspri; }
  void InstallMultiSprinkler (MultiSprinkler *ms)
    { mulspri = ms; }
};


namespace RawEventSupportHose  {

class Caliban  :  public Zeubject, public ZESpatialPhagy
{ public:
  Vect just_now, accum;
  Vect vee[3];
  int phase, count;
  bool slurping;
  f64 above_the_floor_ness, lateral_stretchy_metron;
  Matrix44 default_xform_pmat, default_xform_dmat;
  Matrix44 *geo_truth_pm, *geo_truth_dm;

  Caliban ()  :  Zeubject (),
                 geo_truth_pm (&default_xform_pmat),
                 geo_truth_dm (&default_xform_dmat)
    { Reset (); }

  void Reset ();

  Matrix44 *GeoTruthPointMat ()  const
    { return geo_truth_pm; }
  Matrix44 *GeoTruthDirecMat ()  const
    { return geo_truth_dm; }
  void SetGeoTruthMats (Matrix44 &pm, Matrix44 &dm)
    { geo_truth_pm = &pm;
      geo_truth_dm = &dm;
    }

  void SummonTheDemiurgeCalculon (Matrix44 &out_pmat, Matrix44 &out_dmat);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;
};
// end of 'class Caliban', don't you know.


}  // lights out for namespace RawEventSupportHose


class RawOSCWandParser  :  public RawEventParser
{ public:
  Matrix44 theirs_to_ours_pm, theirs_to_ours_dm;
  RawEventSupportHose::Caliban calibrex;

  RawOSCWandParser ()  :  RawEventParser ()
    { calibrex . SetGeoTruthMats (theirs_to_ours_pm, theirs_to_ours_dm); }

  void Parse (const std::string &path, const lo::Message &m,
              OmNihil *phage = NULL);
};


}  // when your little namespace charm is on the slab, where will it tickle you?


#endif
