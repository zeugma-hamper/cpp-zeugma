#include <OSCWandWaterWorks.hpp>

#include <GraphicsApplication.hpp>
#include <MultiSprinkler.hpp>

#include <conjure-from-toml.h>

#include <lo/lo_cpp.h>

#include <filesystem>

namespace charm
{

namespace fs = std::filesystem;

ZERoomCalibrationEvent::ZERoomCalibrationEvent (std::string_view _prv)
  : ZeEvent (),
    m_provenance(_prv)
{ }

ZERoomCalibrationEvent::ZERoomCalibrationEvent (std::string_view _prv,
                                                Matrix44 const &_pmat,
                                                Matrix44 const &_dmat)
  : ZeEvent (),
    m_provenance(_prv),
    m_point_xform(_pmat),
    m_direc_xform(_dmat)
{ }

void ZERoomCalibrationEvent::EquipWithTransformations (Matrix44 const &_pmat,
                                                       Matrix44 const &_dmat)
{
  m_point_xform = _pmat;
  m_direc_xform = _dmat;
}

std::string const &ZERoomCalibrationEvent::GetProvenance () const
{
  return m_provenance;
}

Matrix44 const &ZERoomCalibrationEvent::GetPointTransform () const
{
  return m_point_xform;
}

Matrix44 const &ZERoomCalibrationEvent::GetDirectionTransorm () const
{
  return m_direc_xform;
}

namespace RawEventSupportHose
{



const std::string RoomTMPCaliban::default_config_dir
  = "/opt/trelopro/config/";
const std::string RoomTMPCaliban::default_config_filebasename
  = "coord-xform-tto.";

bool RoomTMPCaliban::SlurpCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                             const std::string &in_path)
{ std::string dir = default_config_dir;
  std::string pth = "";

  if (! in_path . empty ())
    { fs::path p (in_path);
      if (fs::is_directory (p))
        dir = in_path;
      else if (fs::is_regular_file (p))
        pth = in_path;
      else
        return false;
    }

  if (! pth . empty ())
    return PointAndDirecTransformMatsFromTOML (pth, pmat, dmat);

  if (! fs::directory_entry (fs::path (dir)) . exists ())
    return false;

  size_t /* how we loathe, loathe, loathe size_t... */ scythes
    = default_config_filebasename . size ();

  std::map <fs::file_time_type, fs::directory_entry> sortiful;
  for (auto item  :  fs::directory_iterator (fs::path (dir)))
    if (item . is_regular_file ())
      { std::string fbase
          = item . path () . filename () . string () . substr (0, scythes);
        if (fbase  ==  default_config_filebasename)
          { fs::file_time_type ftt = item . last_write_time ();
            sortiful[ftt] = item;
          }
      }
  if (sortiful . empty ())
    return false;
  pth = sortiful . rbegin ()->second . path () . string ();
  fprintf (stderr, "think the recentest might just be <%s>...\n",
           pth . c_str ());
  return PointAndDirecTransformMatsFromTOML (pth, pmat, dmat);
}


bool RoomTMPCaliban::SpewCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
                                          const std::string &fname,
                                          const std::string &directory_path)
{ f64 *eff = &pmat.a_[0];
  toml::value pm { *eff };
  for (i32 q = 15  ;  q > 0  ;  --q)
    pm . push_back (*++eff);

  eff = &dmat.a_[0];
  toml::value dm { *eff };
  for (i32 q = 15  ;  q > 0  ;  --q)
    dm . push_back (*++eff);

  toml::table tab { { "point_mat", pm }, { "direc_mat", dm } };

  auto timey = std::time (nullptr);
  std::stringstream ss;
  ss << std::put_time (std::localtime (&timey), "%F.%T");
  auto time_s = ss . str ();
  std::replace (time_s . begin (), time_s . end (), ':', '-');

  std::ofstream outfile;
  std::string pth = ((directory_path . empty ()
                      ?  default_config_dir  :  directory_path)
                     +  (fname . empty ()
                         ?  (default_config_filebasename + time_s)  :  fname));
  outfile . open (pth);
  if (outfile . fail ())
    { fprintf (stderr, "hey chief -- couldn't open <%s> for writing.",
               pth . c_str ());
      return false;
    }
  outfile << toml::value (tab);
  outfile . close ();

  return true;
}

void RoomTMPCaliban::SummonTheDemiurgeCalculon (Matrix44 &out_pmat,
                                                Matrix44 &out_dmat)
{ Vect e0 = vee[1] - vee[0];
  f64 d = e0 . NormSelfReturningMag ();
  Vect e1 = e0 . Cross (vee[2] - vee[0]) . Norm ();
  Vect e2 = e0 . Cross (e1);

  Matrix44 ma (INITLESS);
  ma . LoadTranslation (-vee[0]);
  Matrix44 mb;
  mb.m[0][0] = e0.x;  mb.m[0][1] = e1.x;  mb.m[0][2] = e2.x;
  mb.m[1][0] = e0.y;  mb.m[1][1] = e1.y;  mb.m[1][2] = e2.y;
  mb.m[2][0] = e0.z;  mb.m[2][1] = e1.z;  mb.m[2][2] = e2.z;
  Matrix44 mc (INITLESS);
  mc . LoadScale (lateral_stretchy_metron / d);
  Matrix44 md (INITLESS);
  md . LoadTranslation (-0.5 * d, above_the_floor_ness, 0.0);

  out_pmat = ma * mb * mc * md;
  out_dmat = mb;
}

void RoomTMPCaliban::Reset ()
{
  above_the_floor_ness = 653.0;
  lateral_stretchy_metron = 4000.0;
  phase = -1;
  slurping = false;
  accum . Zero ();
  vee[0] . Zero ();  vee[1] . Zero ();  vee[2] . Zero ();
}

i64 RoomTMPCaliban::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (e -> Provenance ()  !=  "wand-0")
    return 0;
  just_now = e -> Loc ();
  if (! slurping)
    { just_now . SpewToStderr ();  fprintf (stderr, "\n");
      return 0;
    }
  accum += just_now;
  ++count;
  fprintf (stderr, "ACCUM [phase %d, count %d]: ", phase, count);
  just_now . SpewToStderr ();
  return 0;
}

i64 RoomTMPCaliban::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  slurping = true;
  accum = just_now;
  count = 1;  // accum already has the most recent, see? already ahead!
  ++phase;
  fprintf (stderr, "\n\n\n--- COLLECTION PHASE %d%d%d ---\n", phase,
           phase, phase);
  return 0;
}

i64 RoomTMPCaliban::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  vee[phase] = accum / (f64)count;
  slurping = false;
  if (phase < 2)
    return 0;
  SummonTheDemiurgeCalculon (xform_pmat, xform_dmat);
  phase = -1;
  fprintf (stderr, "ROOM CALIBRATION O V E R A N D D O N E W I T H\n");
  SpewCoordTransforms (xform_pmat, xform_dmat);

  GraphicsApplication *ga = GraphicsApplication::GetApplication ();
  if (ga)
    {
      ZERoomCalibrationEvent zrce ("room-caliban", xform_pmat, xform_dmat);
      ga->GetSprinkler().Spray(&zrce);
    }

  return -666;
}

}

static int osc_wandler (const char *pth, const char */*typ*/,
                        lo_arg **/*av*/, int /*ac*/,
                        lo_message msg, void *udata)
{
  OSCWandWaterWorks *owww = (OSCWandWaterWorks *)udata;
  owww->Parse (pth, lo::Message (msg));

  return 0;
}

static int eruct_handler (const char *pth, const char *typ,
                          lo_arg **/*av*/, int ac,
                          lo_message /*msg*/, void */*udata*/)
{
  fprintf (stderr, "some other message: PTH=[%s]-TYP=[%s]-RGS[%d]\n",
           pth, typ, ac);
  return 1;
}


OSCWandWaterWorks::OSCWandWaterWorks ()
  :  ZePublicWaterWorks (),
     osc_srv {nullptr},
     temp_sprinkler {nullptr}
{

  lo_server los = lo_server_new ("54345", NULL);
  if (! los)
    fprintf (stderr, "Could not conjure an OSC server -- prob'ly the port.\n");
  else
    {
      osc_srv = new lo::Server (los);
      osc_srv -> add_method ("/events/spatial", NULL, osc_wandler, this);
      osc_srv -> add_method (NULL, NULL, eruct_handler, this);
    }

  RawEventSupportHose::RoomTMPCaliban::SlurpCoordTransforms(theirs_to_ours_pm,
                                                            theirs_to_ours_dm);

  GraphicsApplication *ga = GraphicsApplication::GetApplication();
  assert (ga);
  MultiSprinkler &sp = ga->GetSprinkler();
  calibration_hookup = sp.AppendHandler (this, &OSCWandWaterWorks::HandleRoomCalibrationEvent);
}

OSCWandWaterWorks::~OSCWandWaterWorks ()
{
  calibration_hookup.disconnect();
  delete osc_srv;
  osc_srv = nullptr;
  temp_sprinkler = nullptr;
}

void OSCWandWaterWorks::Drain (MultiSprinkler *_sprinkler)
{
  temp_sprinkler = _sprinkler;

  int gotted = 0;
  if (osc_srv)
    while ((gotted = osc_srv -> recv (0))  >  0)
      { }  // that is to say: as long as they're there, keep 'em moving!

  temp_sprinkler = nullptr;
}

void OSCWandWaterWorks::Parse(std::string_view path, const lo::Message &m)
{
  if (path  !=  "/events/spatial")
    { fprintf (stderr, "RawOSCWandParser: unexpected address -- how'd we get"
               " a message addressed <%s>?", path . data ());
      return;
    }
  if (m . types ()  !=  "shddddddddd")
    { fprintf (stderr, "RawOSCWandParser: mangled or unexpected message "
               " contents -- type boogers weirdly say <%s>...",
               m . types () . c_str ());
      return;
    }

  int ac = m . argc ();
  lo_arg **av = m . argv ();

  std::string wname (&av[0]->s);
  u64 butt_bits = av[1]->i64;

  Vect pos, aim, ovr;
  pos.x = av[2]->f64;
  pos.y = av[3]->f64;
  pos.z = av[4]->f64;

  aim.x = av[5]->f64;
  aim.y = av[6]->f64;
  aim.z = av[7]->f64;

  ovr.x = av[8]->f64;
  ovr.y = av[9]->f64;
  ovr.z = av[10]->f64;

  theirs_to_ours_pm . TransformVectInPlace (pos);
  theirs_to_ours_dm . TransformVectInPlace (aim);
  theirs_to_ours_dm . TransformVectInPlace (ovr);

  WandState &wstate = recent_wands_state[wname];
  // let's assume that no matter what, given that this is a live tracked
  // wand, every new event is a move event.

  ZESpatialMoveEvent *smev = new ZESpatialMoveEvent (wname, pos, aim, ovr);
  u64 bbits = butt_bits, mask = 0x01;
  while (bbits  !=  0)
    { if (bbits & mask)
        { smev -> SetPressureForID (mask, 1.0);
          bbits ^= mask;
        }
      mask <<= 1;
    }

  temp_sprinkler->Spray(smev);

  u64 prev_bbits;
  if (wstate.valid)
    if ((prev_bbits = wstate.button_state)  !=  butt_bits)
      { // something buttonish has changed...
        mask = 0x01;
        bbits = butt_bits;
        while ((prev_bbits ^ bbits)  !=  0)
          { u64 cur_state = bbits & mask;
            if ((prev_bbits & mask)  !=  cur_state)
              { if (cur_state)
                  { ZESpatialHardenEvent *shev
                      = new ZESpatialHardenEvent (*smev, mask);
                    temp_sprinkler->Spray(shev);
                    delete shev;
                  }
                else
                  { ZESpatialSoftenEvent *ssev
                      = new ZESpatialSoftenEvent (*smev, mask);
                    temp_sprinkler->Spray(ssev);
                    delete ssev;
                  }
                prev_bbits ^= (prev_bbits & mask);
                bbits ^= (bbits & mask);
              }
            mask <<= 1;
          }
      }

  wstate.valid = true;
  wstate.event.AdoptParticulars(*smev);
  wstate.button_state = butt_bits;
  delete smev;
}


i64 OSCWandWaterWorks::HandleRoomCalibrationEvent (ZeEvent::ProtoSprinkler::HOOKUP const &,
                                                   ZERoomCalibrationEvent *_event)
{
  if (! _event)
    return 0;

  theirs_to_ours_pm = _event->GetPointTransform();
  theirs_to_ours_dm = _event->GetDirectionTransorm();
  return 0;
}

}
