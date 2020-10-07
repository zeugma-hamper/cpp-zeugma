
#include "RawEventParsing.h"
#include "ZESpatialEvent.h"

#include "Vect.h"

#include <map>
#include <utility>


using namespace charm;


using SEV_WITH_BUTT = std::pair <ZESpatialEvent *, u64>;

static std::map <std::string, SEV_WITH_BUTT> recent_wands_state;


void RawOSCWandParser::Parse (const std::string &path, const lo::Message &m,
                              OmNihil *phage)
{ if (path  !=  "/events/spatial")
    { fprintf (stderr, "RawOSCWandParse: unexpected address -- how'd we get"
               " a message addressed <%s>?", path . c_str ());
      return;
    }
  if (m . types ()  !=  "shddddddddd")
    { fprintf (stderr, "RawOSCWandPArse: mangled or unexpected message "
               " contents -- type boogers weirdly say <%s>...",
               m . types () . c_str ());
      return;
    }

  EventSprinklerGroup *essgee = SprinklerGroup ();

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
  if (essgee)
    essgee -> MetaSpray (smev);

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
                    if (essgee)
                      essgee -> MetaSpray (shev);
                    delete shev;
                  }
                else
                  { ZESpatialSoftenEvent *ssev
                      = new ZESpatialSoftenEvent (*smev, mask);
                    if (phage)
                      ssev -> ProfferAsSnackTo (phage);
                    if (essgee)
                      essgee -> MetaSpray (ssev);
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




using namespace charm::RawEventSupportHose;


void Caliban::Reset ()
{ above_the_floor_ness = 653.0;
  lateral_stretchy_metron = 4000.0;
  phase = -1;
  slurping = false;
  accum . Zero ();
  vee[0] . Zero ();  vee[1] . Zero ();  vee[2] . Zero ();
}


void Caliban::SummonTheDemiurgeCalculon (Matrix44 &out_pmat, Matrix44 &out_dmat)
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


i64 Caliban::ZESpatialMove (ZESpatialMoveEvent *e)
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

i64 Caliban::ZESpatialHarden (ZESpatialHardenEvent *e)
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

i64 Caliban::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (e -> Provenance ()  ==  "wand-0")
    return 0;
  vee[phase] = accum / (f64)count;
  slurping = false;
  if (phase < 2)
    return 0;
  SummonTheDemiurgeCalculon (*geo_truth_pm, *geo_truth_dm);
  phase = -1;
  fprintf (stderr, "CALIBRATION O V E R A N D D O N E W I T H\n");
  return -666;
}
