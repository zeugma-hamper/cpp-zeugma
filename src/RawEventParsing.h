
//
// (c) treadle & loam, provisioners llc
//

#ifndef RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS
#define RAW_EVENT_PARSING_RETAINS_THE_NATURAL_VITAMINS_AND_FRESHNESS


#include "Zeubject.h"
#include "ZESpatialEvent.h"

#include "Bolex.h"

#include "Matrix44.h"

#include "MultiSprinkler.hpp"

#include <lo/lo_cpp.h>


namespace zeugma  {


class RawEventParser  :  public Zeubject
{ public:
  std::vector <OmNihil *> phages;
  EventSprinklerGroup *esg;
  MultiSprinkler mulspri;
  RawEventParser ()  :  Zeubject (), esg (NULL)
    { }
  i64 NumPhages ()  const
    { return phages . size (); }
  OmNihil *NthPhage (i64 ind)
    { return (ind >= 0  &&  ind < phages . size ())  ?  phages[ind]  :  NULL; }
  void AppendPhage (OmNihil *p)
    { if (p)  phages . push_back (p); }
  void RemovePhage (OmNihil *p)
    { auto it = std::find (phages . begin (), phages . end (), p);
      if (it  !=  phages . end ())  phages . erase (it);
    }
  EventSprinklerGroup *SoleSprinklerGroup ()
    { return esg; }
  void InstallSprinklerGroup (EventSprinklerGroup *sprigrou)
    { esg = sprigrou; }
  MultiSprinkler &SoleMultiSprinkler ()
    { return mulspri; }
};


namespace RawEventSupportHose  {

class Caliban  :  public Zeubject, public ZESpatialPhagy
{ public:
  Caliban ()  :  Zeubject ()
    { }
};

class RoomCaliban  :  public Caliban
{ public:
  Vect just_now, accum;
  Vect vee[3];
  int phase, count;
  bool slurping;
  f64 above_the_floor_ness, lateral_stretchy_metron;
  Matrix44 default_xform_pmat, default_xform_dmat;
  Matrix44 *geo_truth_pm, *geo_truth_dm;

  RoomCaliban ()  :  Caliban (),
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
// end of 'class RoomCaliban', don't you know.


class TableCaliban  :  public Caliban
{ public:
  std::vector <Vect> captures;
  bool slurping;
  TableCaliban ()  :  Caliban ()
    { Reset (); }

  void Reset ();

  Vect ShabbyTableInferencizing ();

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;
};


}  // lights out for namespace RawEventSupportHose


class RawOSCWandParser  :  public RawEventParser
{ public:
  static const std::string default_config_dir;
  static const std::string default_config_filebasename;
  Matrix44 theirs_to_ours_pm, theirs_to_ours_dm;
  RawEventSupportHose::RoomCaliban room_calibrex;
  RawEventSupportHose::TableCaliban table_calibrex;

  RawOSCWandParser ()  :  RawEventParser ()
    { room_calibrex . SetGeoTruthMats (theirs_to_ours_pm, theirs_to_ours_dm); }

  static bool SlurpCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                    const std::string &pathy = "");
  static bool SpewCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                   const std::string &fname = "",
                                   const std::string &directory_path = "");
  bool HooverCoordTransforms ()
    { return SlurpCoordTransforms (theirs_to_ours_pm, theirs_to_ours_dm); }
  bool EmitCoordTransforms ()
    { return SpewCoordTransforms (theirs_to_ours_pm, theirs_to_ours_dm); }

  void Parse (const std::string &path, const lo::Message &m,
              OmNihil *phage = NULL);
};


class RawMouseParser  :  public RawEventParser
{ public:

  void MouseMove (const std::string &nm, f64 x_nrm, f64 y_nrm,
                  Bolex *cam, OmNihil *phage = NULL);
  void MouseButt (const std::string &nm, u64 which_butt, f64 prssr,
                  OmNihil *phage = NULL);
};


}  // when your little n'space zeugma is on the slab, where will it tickle you?


#endif
