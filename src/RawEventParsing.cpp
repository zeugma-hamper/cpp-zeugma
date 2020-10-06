
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

