
//
// (c) treadle & loam, provisioners llc
//

#include "RawEventParsing.h"
#include "ZESpatialEvent.h"

#include "Vect.h"

#include "conjure-from-toml.h"

#include <map>
#include <vector>
#include <utility>

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <toml.hpp>

#include <filesystem>


using namespace zeugma;

namespace fs = std::filesystem;


const std::string RawOSCWandParser::default_config_dir
  = "/opt/trelopro/config/";
const std::string RawOSCWandParser::default_config_filebasename
  = "coord-xform-tto.";


bool RawOSCWandParser::SlurpCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
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


bool RawOSCWandParser::SpewCoordTransforms (Matrix44 &pmat, Matrix44 &dmat,
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


using SEV_WITH_BUTT = std::pair <ZESpatialEvent *, u64>;

static std::map <std::string, SEV_WITH_BUTT> recent_wands_state;


void RawOSCWandParser::Parse (const std::string &path, const lo::Message &m,
                              OmNihil *phage)
{ if (path  !=  "/events/spatial")
    { fprintf (stderr, "RawOSCWandParser: unexpected address -- how'd we get"
               " a message addressed <%s>?", path . c_str ());
      return;
    }
  if (m . types ()  !=  "shddddddddd")
    { fprintf (stderr, "RawOSCWandParser: mangled or unexpected message "
               " contents -- type boogers weirdly say <%s>...",
               m . types () . c_str ());
      return;
    }

  EventSprinklerGroup *essgee = SoleSprinklerGroup ();
  MultiSprinkler &emmess = SoleMultiSprinkler ();

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

  SEV_WITH_BUTT *prev_state = NULL;
  try { prev_state = &recent_wands_state[wname]; }
  catch (std::out_of_range &exc) { }

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

  if (phage)
    smev -> ProfferAsSnackTo (phage);
  for (OmNihil *ph  :  phages)
    smev -> ProfferAsSnackTo (ph);
  if (essgee)
    essgee -> MetaSpray (smev);
  emmess . Spray (smev);

  u64 prev_bbits;
  if (prev_state)
    if ((prev_bbits = prev_state->second)  !=  butt_bits)
      { // something buttonish has changed...
        mask = 0x01;
        bbits = butt_bits;
        while ((prev_bbits ^ bbits)  !=  0)
          { u64 cur_state = bbits & mask;
            if ((prev_bbits & mask)  !=  cur_state)
              { if (cur_state)
                  { ZESpatialHardenEvent *shev
                      = new ZESpatialHardenEvent (*smev, mask);
                    if (phage)
                      shev -> ProfferAsSnackTo (phage);
                    for (OmNihil *ph  :  phages)
                      shev -> ProfferAsSnackTo (ph);
                    if (essgee)
                      essgee -> MetaSpray (shev);
                    emmess . Spray (shev);
                    delete shev;
                  }
                else
                  { ZESpatialSoftenEvent *ssev
                      = new ZESpatialSoftenEvent (*smev, mask);
                    if (phage)
                      ssev -> ProfferAsSnackTo (phage);
                    for (OmNihil *ph  :  phages)
                      ssev -> ProfferAsSnackTo (ph);
                    if (essgee)
                      essgee -> MetaSpray (ssev);
                    emmess . Spray (ssev);
                    delete ssev;
                  }
                prev_bbits ^= (prev_bbits & mask);
                bbits ^= (bbits & mask);
              }
            mask <<= 1;
          }
      }

  if (prev_state)
    delete prev_state->first;
  recent_wands_state[wname] = std::make_pair (smev, butt_bits);
}

// HEY-YO: the delete instances foregoing probably will want to stay
// even after mem-mangament is implemented, but... let's remember to
// think about it. Okay, Chet?
// HI: Don't call me Chet, Buddy!




using namespace zeugma::RawEventSupportHose;


void RoomCaliban::Reset ()
{ above_the_floor_ness = 653.0;
  lateral_stretchy_metron = 4000.0;
  phase = -1;
  slurping = false;
  accum . Zero ();
  vee[0] . Zero ();  vee[1] . Zero ();  vee[2] . Zero ();
}


void RoomCaliban::SummonTheDemiurgeCalculon (Matrix44 &out_pmat,
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


i64 RoomCaliban::ZESpatialMove (ZESpatialMoveEvent *e)
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

i64 RoomCaliban::ZESpatialHarden (ZESpatialHardenEvent *e)
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

i64 RoomCaliban::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  vee[phase] = accum / (f64)count;
  slurping = false;
  if (phase < 2)
    return 0;
  SummonTheDemiurgeCalculon (*geo_truth_pm, *geo_truth_dm);
  phase = -1;
  fprintf (stderr, "ROOM CALIBRATION O V E R A N D D O N E W I T H\n");
  RawOSCWandParser::SpewCoordTransforms (*geo_truth_pm, *geo_truth_dm);
  return -666;
}



void TableCaliban::Reset ()
{ captures . clear ();
  slurping = false;
}


Vect TableCaliban::ShabbyTableInferencizing ()
{ const std::vector <Vect> &pts = captures;
  i64 cnt = pts . size ();
  i64 a = 0, b = cnt / 3, c = 2 * cnt / 3;

  Vect center, norman;
  i64 largess = 0;
  for (  ;  a < cnt  ;  ++a, ++b, ++c)
    { const Vect &v = pts[a];
      Vect leg1 = (pts[b%cnt] - v) . Norm ();
      Vect leg2 = (pts[c%cnt] - v) . Norm ();
      Vect n = leg1 . Cross (leg2);
      if (n . AutoDot ()  <  0.01)
        continue;
      if (n . Dot (Vect::yaxis)  <  0)
        norman -= n;
      else
        norman += n;
      center += v;
      ++largess;
    }
  center /= (f64)largess;
  norman . NormSelf ();

  fprintf (stderr,
           "ShAvNo: able to use %ld out of %ld three-way combinations...\n"
           "Anyway, ",
           largess, cnt);
  norman . SpewToStderr ();
  fprintf (stderr, " was the brilliant ave'd-normal outcome.\n");

  struct sac { i64 ind; f64 dst; f64 ang; Vect pnt; };

  f64 mind = CHRM_MAX_F64;
  f64 maxd = -CHRM_MAX_F64;
  f64 frc = 0.0;
  std::vector <sac> sacpnt (cnt);
  for (a = 0  ;  a < cnt  ;  ++a)
    { sacpnt[a].ind = a;
      f64 d = norman . Dot (pts[a] - center);
      if (d < mind)
        mind = d;
      else if (d > maxd)
        maxd = d;
      frc += (sacpnt[a].dst = d);
    }
  frc /= (f64)cnt;

  // er... no hooke's law simulation actually necessary here, as we can
  // achieve equilibrium in one go by displacnig cnt by d_avg (aka frc,
  // above) along the normal.

  Vect offset = frc * norman;
  center += offset;

  // well, heck. now we have a plane! should we... dare we project each
  // collected point down (or up) onto this newly canonical plane? yes:
  // because for corner-finding we don't want to be misled by 'spurious'
  // (yes, yes...) y-axis jigglings.
  for (a = 0  ;  a < cnt  ;  ++a)
    sacpnt[a].dst = ((sacpnt[a].pnt = pts[a] + offset) - center) . Mag ();


  // if we now "sweep around" the plane, can we find the corners?
  Vect fko = (sacpnt[0].pnt - center) . Norm ();
  Vect fku = norman . Cross (fko) . Norm ();
  Vect tmp;
  for (a = 0  ;  a < cnt  ;  ++a)
    { tmp = sacpnt[a].pnt - center;
      sacpnt[a].ang = 180.0
        +  180.0 / M_PI * atan2 (tmp . Dot (fku), tmp . Dot (fko));
    }
  std::sort (sacpnt . begin (), sacpnt . end (),
             [] (const sac &a, const sac &b) { return a.dst > b.dst; });

  sac crnrs[4];
  crnrs[0] = sacpnt[0];
  a = 1;
  i64 w, q = 0;
  Vect goodlegs[4];  // yes, yes: could be three instead of four.
  goodlegs[0] = (crnrs[0].pnt - center) . Norm ();
  while (q < 3  &&  a < cnt)
    { Vect newleg = (sacpnt[a].pnt - center) . Norm ();
      for (w = q  ;  w >= 0  ;  --w)
        { f64 blang = 180.0 / M_PI * acos (newleg . Dot (goodlegs[w]));
          if (blang < 15.0)  // figger fi'teen degrees is furnuf...
            break;
        }
      if (w  <  0)  // angularly distant from all winners foregoing
        { ++q;
          crnrs[q] = sacpnt[a];
          goodlegs[q] = newleg;
        }
      ++a;
    }
  Vect splend_center;
  if (q != 4)
    { fprintf (stderr, "TableCaliban::ShabbyTableInferencizing -- "
               "whole heapa trouble...\n");
    }
  else
    { splend_center
        = 0.25 * (crnrs[0].pnt + crnrs[1].pnt + crnrs[2].pnt + crnrs[3].pnt);
      splend_center -= norman . Dot (splend_center - center) * norman;
    }

  return norman;
}

i64 TableCaliban::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (e -> Provenance ()  !=  "wand-0")
    return 0;
  Vect pos = e -> Loc ();
  if (! slurping)
    { pos . SpewToStderr ();  fprintf (stderr, "\n");
      return 0;
    }
  captures . push_back (pos);
  pos . SpewToStderr ();
  fprintf (stderr, " just now ACCUM'D [count %d]\n", captures . size ());
  return 0;
}

i64 TableCaliban::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  fprintf (stderr, "\n\n\n--- READY TO COLLECT TABULARLY ---\n");
  return 0;
}

i64 TableCaliban::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  if (slurping  ==  false)
    { slurping = true;
      return 0;
    }
  slurping = false;
  ShabbyTableInferencizing ();
  fprintf (stderr, "O V E R A N D D O N E W I T H, IS TABLE CALIBRATION\n");
  return -666;
}



void RawMouseParser::MouseMove (const std::string &nm, f64 x_nrm, f64 y_nrm,
                                Bolex *cam, OmNihil *phage)
{ EventSprinklerGroup *essgee = SoleSprinklerGroup ();
  MultiSprinkler &emmess = SoleMultiSprinkler ();
  SEV_WITH_BUTT *prev_state = NULL;
  try { prev_state = &recent_wands_state[nm]; }
  catch (std::out_of_range &exc) { }

  Vect thr = cam -> ViewLoc ()  +  cam -> ViewDist () * cam -> ViewAim ();
  f64 wid = cam -> IsProjectionTypeOrthographic ()  ?  cam -> ViewOrthoWid ()
    :  cam -> ViewDist () * 2.0 * tan (0.5 * cam -> ViewHorizAngle ());
  f64 hei = cam -> IsProjectionTypeOrthographic ()  ?  cam -> ViewOrthoHei ()
    :  cam -> ViewDist () * 2.0 * tan (0.5 * cam -> ViewVertAngle ());
  Vect ovr = cam -> ViewAim () . Cross (cam -> ViewUp ()) . Norm ();
  Vect upp = ovr . Cross (cam -> ViewAim ());

  thr += (x_nrm - 0.5) * wid * ovr  +  (y_nrm - 0.5) * hei * upp;
  Vect frm = cam -> IsProjectionTypePerspective ()  ?  cam -> ViewLoc ()
    :  thr - cam -> ViewDist () * cam -> ViewAim ();

  ZESpatialMoveEvent *smev
    = new ZESpatialMoveEvent (nm, frm, (thr - frm) . Norm (), ovr);
  if (phage)
    smev -> ProfferAsSnackTo (phage);
  for (OmNihil *ph  :  phages)
    smev -> ProfferAsSnackTo (ph);
  if (essgee)
    essgee -> MetaSpray (smev);
  emmess . Spray (smev);

  if (prev_state)
    delete prev_state->first;
  recent_wands_state[nm] = std::make_pair (smev, prev_state->second);
}


void RawMouseParser::MouseButt (const std::string &nm, u64 which_butt, f64 prssr,
                                OmNihil *phage)
{ EventSprinklerGroup *essgee = SoleSprinklerGroup ();
  MultiSprinkler &emmess = SoleMultiSprinkler ();
  SEV_WITH_BUTT *prev_state = NULL;
  try { prev_state = &recent_wands_state[nm]; }
  catch (std::out_of_range &exc) { }

  u64 evlv_butt = prev_state  ?  prev_state->second  :  0;
  ZESpatialPressureEvent *spev = NULL;
  if ((evlv_butt & which_butt) == 0  &&  prssr > 0.0)
    { evlv_butt ^= which_butt;
      ZESpatialHardenEvent *shev = prev_state
        ?  new ZESpatialHardenEvent (*prev_state->first, which_butt, prssr)
        :  new ZESpatialHardenEvent (nm, which_butt, prssr);
      spev = shev;
    }
  else if ((evlv_butt & which_butt) == which_butt  &&  prssr < 1.0)
    { evlv_butt ^= which_butt;
      ZESpatialSoftenEvent *ssev = prev_state
        ?  new ZESpatialSoftenEvent (*prev_state->first, which_butt, prssr)
        :  new ZESpatialSoftenEvent (nm, which_butt, prssr);
      spev = ssev;
    }
  else
    { // odd for sure: button was already in the 'new' state...
      return;
    }

  if (phage)
    spev -> ProfferAsSnackTo (phage);
  for (OmNihil *ph  :  phages)
    spev -> ProfferAsSnackTo (ph);
  if (essgee)
    essgee -> MetaSpray (spev);
  emmess . Spray (spev);

  if (prev_state)
    delete prev_state->first;
  recent_wands_state[nm] = std::make_pair (spev, evlv_butt);
}
