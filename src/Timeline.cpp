
#include "Timeline.h"

#include "SilverScreen.h"

#include "SumZoft.h"
#include "TrGrappler.h"

#include "TexturedRenderable.hpp"


Timeline::Timeline ()  :  Zeubject (), Node (),
                          width (100.0), thickth (10.0), rep_duration (1.0),
                          cine_receiver (NULL)
{ AppendChild (play_nub = new Node);
  AppendChild (hover_nub = new Node);

  TextureParticulars tepa
    = CreateTexture2D ("../play-nub.png", DefaultTextureFlags);
  TexturedRenderable *texre = new TexturedRenderable (tepa);
texre->SetAdjColor(ZeColor(1.0,0.0,0.0));
  play_nub -> AppendRenderable (texre);

  lft_end . MakeBecomeLikable ();  rgt_end . MakeBecomeLikable ();
  upp_hlf . MakeBecomeLikable ();  dwn_hlf . MakeBecomeLikable ();

  AppendRenderable (track_rend = new PolygonRenderable);
  track_rend -> AppendVertex (SumVect (lft_end, dwn_hlf));
  track_rend -> AppendVertex (SumVect (lft_end, upp_hlf));
  track_rend -> AppendVertex (SumVect (rgt_end, upp_hlf));
  track_rend -> AppendVertex (SumVect (rgt_end, dwn_hlf));

  loc . MakeBecomeLikable ();
  TrGrappler *trg = new TrGrappler (loc);
  AppendGrappler (trg);
}


Timeline::~Timeline ()
{ }



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
}


void Timeline::SetRepDuration (f64 dur)
{ rep_duration = dur;
  // and presumably enfiddle various graphical representations...
}

void Timeline::SetPlayTime (f64 t)
{ play_time = t;
  pnub_pos . Set (width * (t / rep_duration - 0.5) * Vect::xaxis);
  // fondle play_nub
}


void Timeline::EstablishCineReceiver (SilverScreen *essess)
{ if (cine_receiver)
    cine_receiver -> DetachTimeline (this);

  if (cine_receiver = essess)
    { cine_receiver -> AttachTimeline (this); }
}
