
#ifndef SILVER_SCREEN_FOR_PHOTONS_THROUGH_SILVER_HALIDE_IN_JELLO
#define SILVER_SCREEN_FOR_PHOTONS_THROUGH_SILVER_HALIDE_IN_JELLO


#include "Zeubject.h"

#include "PlatonicMaes.h"

#include "ZEBulletinEvent.h"
#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"

#include "InterpZoft.h"

#include <Node.hpp>
#include <Matte.hpp>
#include <MattedVideoRenderable.hpp>

#include "tamparams.h"

#include "Timeline.h"

#include <Frontier.hpp>

#include <vector>


using namespace charm;


struct WoCoLoBbox {
  const ClipInfo *clnf;
  Vect cntr;
  f64 widt, heig;
  Vect lr, ur, ul, ll;
};


class SilverScreen  :  public Node
{ public:
  const FilmInfo &finf;
  MattedVideoRenderable *vren;
  ch_ptr <VideoPipeline> vpip;
  RectRenderableFrontier *frtr;
  InterpColor scr_fader;
  Timeline *timmy;
  ClipList nascent_atoms;
  std::vector <WoCoLoBbox> natom_bboxes;

  SilverScreen (MattedVideoRenderable *vr,
                ch_ptr <VideoPipeline> vp,
                const FilmInfo &fi);

  const std::string &Name ()
    { return finf.name; }

  MattedVideoRenderable *Flick ()
    { return vren; }

  f64 AspectRatio ();
  Vect WoCoCenter ();
  std::pair <f64, f64> WoCoWidHei ();

  void FadeUp ();
  void FadeDown ();

  bool IsPaused ();

  void Pause (bool including_post_pause_rigamarole = true);
  void Play ();
  void TogglePlayPause ();

  bool StepBy (i32 num_frames);

  f64 Duration ();
  f64 CurTimestamp ();

  i64 NumNascentAtoms ()
    { return nascent_atoms . size (); }
  const ClipInfo *NthNascentAtom (i64 ind);
  WoCoLoBbox *NthNatomBbox (i64 ind);
  WoCoLoBbox *NatomBboxByClip (const ClipInfo *clinf);
  void ReckonNatomBboxes ();

  bool JumpToTime (f64 tstamp);

  bool ScootToTime (f64 tstamp);
  bool ScootToNextClip ();
  bool ScootToPrevClip ();

  void AttachTimeline (Timeline *tl);
  void DetachTimeline (Timeline *tl);
};


#endif
