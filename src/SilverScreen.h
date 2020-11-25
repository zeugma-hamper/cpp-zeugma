
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
#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include "tamparams.h"

#include <Frontier.hpp>

#include <vector>


using namespace charm;


class SilverScreen  :  public Node
{ public:
  const FilmInfo &finf;
//  VideoRenderable *vren;
  MattedVideoRenderable *vren;
  RectRenderableFrontier *frtr;
  InterpColor scr_fader;
//  ZoftColor scr_fader;

  const std::string &Name ()
    { return finf.name; }

//  SilverScreen (VideoRenderable *vr,
  SilverScreen (MattedVideoRenderable *vr,
                const FilmInfo &fi)  :  Node (), vren (vr), finf (fi)
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
};


#endif
