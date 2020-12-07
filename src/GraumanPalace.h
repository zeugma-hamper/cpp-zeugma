
#ifndef GRAUMAN_PALACE_FOR_THE_VERY_SIDNEYEST_SPECTACLE
#define GRAUMAN_PALACE_FOR_THE_VERY_SIDNEYEST_SPECTACLE


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
#include "LinePileRenderable.h"

#include "SilverScreen.h"
#include "Timeline.h"

#include "tamparams.h"

#include <Frontier.hpp>

#include <vector>


using namespace charm;


class GraumanPalace  :  public Zeubject, public Node,
                        public ZESpatialPhagy,
                        public ZEYowlPhagy,
                        public ZEBulletinPhagy
{ public:
  PlatonicMaes *backing_maes;
  std::vector <SilverScreen *> screens;
  f64 flick_wid, flick_spacing;
  Vect ovr, upp, nrm;
  Node *slider;
  InterpVect push_depth;
  InterpVect ltrl_slide;
  i64 now_showing;

  std::string effecting_pushback;
  Vect pb_estab_loc;
  i64 pb_estab_flick;
  f64 pb_depth_scale, pb_slide_scale;
  f64 pb_max_push, pb_max_pull;

  Timeline *sole_tline;
LinePileRenderable *elpyar;
Node *elpy_no;

  GraumanPalace ();

  const Vect &Over ()  const
    { return ovr; }
  const Vect &Up ()  const
    { return upp; }
  const Vect &Norm ()  const
    { return nrm; }

  void SetOverUp (const Vect &o, const Vect &u)
    { ovr = o . Norm ();  upp = u . Norm ();  nrm = ovr . Cross (upp);
      sole_tline -> SetOverAndUp (ovr, upp);
    }
  void SetOverNorm (const Vect &o, const Vect &n)
    { ovr = o . Norm ();  nrm = n . Norm ();  ovr = upp . Cross (nrm);
      sole_tline -> SetOverAndUp (ovr, upp);
    }

  i64 NumSilverScreens ()  const
    { return screens . size (); }
  i64 NumFlicks ()  const
    { return NumSilverScreens (); }

  SilverScreen *NthSilverScreen (i64 ind);
  //VideoRenderable *NthFlick (i64 ind);
  MattedVideoRenderable *NthFlick (i64 ind);

  SilverScreen *CurSilverScreen ()
    { return NthSilverScreen (now_showing); }
  //VideoRenderable *CurFlick ()
  MattedVideoRenderable *CurFlick ()
    { return NthFlick (now_showing); }

  void ImportExhibitionRoster (const std::vector <FilmInfo> &fimmz);

  void JumpToFlick (i64 which_flick);
  void JumpToNextFlick ()
    { JumpToFlick (now_showing + 1); }
  void JumpToPrevFlick ()
    { JumpToFlick (now_showing - 1); }
  void JumpToRandomFlick ()
    { JumpToFlick (i64( (f64(screens . size ()) - 0.0001) * drand48 ())); }

  void TogglePlayPause ();

  void EngagePushback ();
  void ReleasePushback ();

  i64 RummageInCurrentFlick (ZESpatialMoveEvent *e);
  i64 PounceInCurrentFlick (ZESpatialHardenEvent *e);

  i64 ZESpatialMove (ZESpatialMoveEvent *e);
  i64 ZESpatialHarden (ZESpatialHardenEvent *e);
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e);

  i64 ZEYowlAppear (ZEYowlAppearEvent *e);
};


#endif
