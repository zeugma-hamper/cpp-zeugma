
#ifndef THE_MIGHTY_OSC_RIVER
#define THE_MIGHTY_OSC_RIVER


#include <ZePublicWaterWorks.hpp>

#include <ZESpatialEvent.h>

#include <Matrix44.h>


namespace lo
{
  class Server;
  class Message;
}


namespace zeugma  {


class ZERoomCalibrationEvent : public ZeEvent
{
  ZE_EVT_TUMESCE(ZERoomCalibration, Ze);

  ZERoomCalibrationEvent (std::string_view _prv);

  ZERoomCalibrationEvent (std::string_view _prv,
                      Matrix44 const &_pmat,
                      Matrix44 const &_dmat);

  void EquipWithTransformations (Matrix44 const &_pmat,
                                 Matrix44 const &_dmat);

  std::string const &GetProvenance () const;

  Matrix44 const &GetPointTransform () const;
  Matrix44 const &GetDirectionTransorm () const;

 protected:

  std::string m_provenance;
  Matrix44 m_point_xform;
  Matrix44 m_direc_xform;
};

namespace RawEventSupportHose  {

class TMPCaliban  :  public Zeubject, public ZESpatialPhagy
{ public:
  TMPCaliban ()  :  Zeubject ()
    { }
};

class RoomTMPCaliban  :  public TMPCaliban
{ public:
  Vect just_now, accum;
  Vect vee[3];
  int phase, count;
  bool slurping;
  f64 above_the_floor_ness, lateral_stretchy_metron;
  Matrix44 xform_pmat, xform_dmat;

  static const std::string default_config_dir;
  static const std::string default_config_filebasename;

  RoomTMPCaliban ()  :  TMPCaliban ()
    { Reset (); }

  static bool SlurpCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                    const std::string &pathy = "");
  static bool SpewCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                   const std::string &fname = "",
                                   const std::string &directory_path = "");
  bool HooverCoordTransforms ()
    { return SlurpCoordTransforms (xform_pmat, xform_dmat); }
  bool EmitCoordTransforms ()
    { return SpewCoordTransforms (xform_pmat, xform_dmat); }

  void Reset ();

  Matrix44 *GeoTruthPointMat ()  const
    { return const_cast<Matrix44 *> (&xform_pmat); }
  Matrix44 *GeoTruthDirecMat ()  const
    { return const_cast<Matrix44 *> (&xform_dmat); }

  void SummonTheDemiurgeCalculon (Matrix44 &out_pmat, Matrix44 &out_dmat);

  i64 ZESpatialMove   (ZESpatialMoveEvent *e)    override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;
};
// end of 'class RoomTMPCaliban', don't you know.


class TableTMPCaliban  :  public TMPCaliban
{ public:
  std::vector <Vect> captures;
  bool slurping;
  TableTMPCaliban ()  :  TMPCaliban ()
    { Reset (); }

  void Reset ();

  Vect ShabbyTableInferencizing ();

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;
};
}

struct WandState
{
  ZESpatialEvent event;
  u64 button_state = 0;
  bool valid = false;
};

class OSCWandWaterWorks : public ZePublicWaterWorks
{
 public:
  Matrix44 theirs_to_ours_pm, theirs_to_ours_dm;
  lo::Server *osc_srv;
  std::map<std::string, WandState> recent_wands_state;
  ZeEvent::ProtoSprinkler::HOOKUP calibration_hookup;
  MultiSprinkler *temp_sprinkler;

  OSCWandWaterWorks ();
  ~OSCWandWaterWorks () override;

  void Drain (MultiSprinkler *_sprinkler) override;

  void Parse (std::string_view path, const lo::Message &m);

  i64 HandleRoomCalibrationEvent (ZeEvent::ProtoSprinkler::HOOKUP const &_conn,
                                  ZERoomCalibrationEvent *_event);
};


}


#endif  //THE_MIGHTY_OSC_RIVER
