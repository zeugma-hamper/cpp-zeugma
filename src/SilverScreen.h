
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


class SilverScreen  :  public Node
{ public:
  const FilmInfo &finf;
  MattedVideoRenderable *vren;
  ch_ptr <VideoPipeline> vpip;
  RectRenderableFrontier *frtr;
  InterpColor scr_fader;
  Timeline *timmy;

  const std::string &Name ()
    { return finf.name; }

//  SilverScreen (VideoRenderable *vr,
  SilverScreen (MattedVideoRenderable *vr,
                ch_ptr <VideoPipeline> vp,
                const FilmInfo &fi)  :  Node (),
                                        finf (fi), vren (vr), vpip (vp),
                                        timmy (NULL)
    { frtr = new RectRenderableFrontier (vr, Vect::zerov, 1.0, 1.0);
      SetFrontier (frtr);
      scr_fader . SetInterpTime (Tamparams::Current ()->pb_snapback_fade_time);
      scr_fader . PointA () . Set (ZeColor (1.0, 1.0, 1.0, 0.0));
      scr_fader . PointB () . Set (ZeColor (1.0));
      scr_fader . Finish ();
      scr_fader . MakeBecomeLikable ();
      vren -> AdjColorZoft () . BecomeLike (scr_fader);
    }

  void FadeUp ();
  void FadeDown ();

  void Pause ();
  void Play ();
  void TogglePlayPause ();

  bool StepBy (i32 num_frames);

  f64 Duration ();
  f64 CurTimestamp ();

  bool JumpToTime (f64 tstamp);

  bool ScootToTime (f64 tstamp);
  bool ScootToNextClip ();
  bool ScootToPrevClip ();

  void AttachTimeline (Timeline *tl);
  void DetachTimeline (Timeline *tl);
};


#endif
