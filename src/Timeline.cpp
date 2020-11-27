
#include "Timeline.h"

#include "SilverScreen.h"

#include "SumZoft.h"
#include "TrGrappler.h"
#include "CoGrappler.h"

#include "TexturedRenderable.hpp"

#include "vector_interop.hpp"


Timeline::Timeline ()  :  Zeubject (), Node (),
                          ovr (Vect::xaxis), upp (Vect::yaxis),
                          width (100.0), thickth (10.0), rep_duration (1.0),
                          cine_receiver (NULL)
{ AppendChild (play_nub = new Node);
  AppendChild (hover_nub = new Node);

  lft_end . MakeBecomeLikable ();  rgt_end . MakeBecomeLikable ();
  upp_hlf . MakeBecomeLikable ();  dwn_hlf . MakeBecomeLikable ();

// enmaken the play nub
  TextureParticulars tepa
    = CreateTexture2D ("../play-nub.png", DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (tepa);
texre->SetAdjColor(ZeColor(1.0,0.0,0.0));
  play_nub -> AppendRenderable (texre);

// befabricate the hover nub
  hover_ren = new LinePileRenderable;
  hover_nub -> AppendRenderable (hover_ren);
  hover_nub -> Translate (hnub_pos);

  AppendRenderable (track_rend = new PolygonRenderable);
  track_rend -> AppendVertex (SumVect (lft_end, dwn_hlf));
  track_rend -> AppendVertex (SumVect (lft_end, upp_hlf));
  track_rend -> AppendVertex (SumVect (rgt_end, upp_hlf));
  track_rend -> AppendVertex (SumVect (rgt_end, dwn_hlf));
  track_rend -> SetFillColor (ZeColor (0.35, 1.0));

  CoGrappler *cgr = new CoGrappler ();
  cgr -> SetName ("orientation");
  AppendGrappler (cgr);

  shift . SetInterpTime (0.3);
  TrGrappler *trg = new TrGrappler (shift);
  AppendGrappler (trg);
}


Timeline::~Timeline ()
{ }



void Timeline::SetOverAndUp (const Vect &o, const Vect &u)
{ if (CoGrappler *cgr
      = dynamic_cast <CoGrappler *> (FindGrappler ("orientation")))
    cgr -> SetViaOverAndUp (o, u);
  ovr = o . Norm ();
  upp = u . Norm ();
}


void Timeline::SetWidthAndThickth (f64 wi, f64 th)
{ Vect prone = 0.5 * (width = wi) * Vect::xaxis;
  Vect plumb = 0.5 * (thickth = th) * Vect::yaxis;

  lft_end . Set (-prone);
  rgt_end . Set (prone);
  dwn_hlf . Set (-plumb);
  upp_hlf . Set (plumb);

  play_nub -> ClearTransforms ();
  play_nub -> Scale (5.0 * th);
  play_nub -> Translate (1.1 * plumb);
  play_nub -> Translate (pnub_pos);


  hover_ren -> ClearAllLines ();
  Vect p1 = 0.5 * plumb;
  Vect p2 = 1.1 * plumb;
  Vect diag_l = 0.4 * thickth * Vect::yaxis  -  0.2 * thickth * Vect::xaxis;
  Vect diag_r = 0.4 * thickth * Vect::yaxis  +  0.2 * thickth * Vect::xaxis;

  hover_ren -> AppendLine (p2 + diag_l, p2);
  hover_ren -> AppendLine (p2 + diag_r, p2);
  hover_ren -> AppendLine (p2, -p2);
  hover_ren -> AppendLine (-p2 - diag_l, -p2);
  hover_ren -> AppendLine (-p2 - diag_r, -p2);
}


void Timeline::SetRepDuration (f64 dur)
{ rep_duration = dur;
  // and presumably enfiddle various graphical representations?
}

void Timeline::SetPlayTime (f64 pt)
{ play_time = pt;
  pnub_pos . Set (width * (pt / rep_duration - 0.5) * Vect::xaxis);
}

void Timeline::SetHoverTime (f64 ht)
{ hover_time = ht;
  hnub_pos . Set (width * (ht / rep_duration - 0.5) * Vect::xaxis);
}


void Timeline::EstablishCineReceiver (SilverScreen *essess)
{ if (cine_receiver)
    cine_receiver -> DetachTimeline (this);

  if (cine_receiver = essess)
    { cine_receiver -> AttachTimeline (this); }
}


bool Timeline::TimeFromSpatialPointing (ZESpatialEvent *e, f64 &out_time)
{ if (! e)
    return false;

  Vect hitp (INITLESS);
  const Transformation &tranny = GetAbsoluteTransformation ();
  Matrix44 emm = from_glm (tranny.model);
  Vect cnt = emm . TransformVect (Vect::zerov);

  if (G::RayPlaneIntersection (e -> Loc (), e -> Aim (),
                               cnt, ovr . Cross (upp), &hitp))
    { hitp -= cnt;
      f64 ov = hitp . Dot (ovr);
      f64 up = hitp . Dot (upp);
      if (ov < 0.65 * width  &&  ov > -0.65 * width
          &&  up < 20.0 * thickth  &&  up > -20.0 * thickth)
        { f64 t = ov / width  +  0.5;
          if (t < 0.0)       t = 0.0;
          else if (t > 1.0)  t = 1.0;
          out_time = t * rep_duration;
          return true;
        }
    }
  return false;
}


i64 Timeline::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;
  f64 t;
  if (TimeFromSpatialPointing (e, t))
    SetHoverTime (t);
  return 0;
}

i64 Timeline::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (! e)
    return -1;
  f64 t;
  if (TimeFromSpatialPointing (e, t))
    { SetPlayTime (t);
      if (cine_receiver)
        cine_receiver -> JumpToTime (t);
    }
  return 0;
}

i64 Timeline::ZESpatialSoften (ZESpatialSoftenEvent *e)
{
  return 0;
}

i64 Timeline::ZEYowlAppear (ZEYowlAppearEvent *e)
{
  return 0;
}

