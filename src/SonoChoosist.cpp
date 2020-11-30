
#include "SonoChoosist.h"


using namespace charm;


SonoChoosist::SonoChoosist (PlatonicMaes *maes)  :  Alignifer ()
{ if (! maes)
    { wid = 200.0;  hei = 40.0;  brd_thc = 5.0; }
  else
    { wid = 0.9 * maes -> Width ();
      hei = 0.125 * wid;
      brd_thc = 0.5 * (maes -> Width ()  -  wid);
      AlignToMaes (maes);
    }

 AppendRenderable (brdr_re = new PolygonRenderable);
 Vect hrz = 0.5 * wid * Vect::xaxis;
 Vect vrt = 0.5 * hei * Vect::yaxis;

}


void SonoChoosist::Furl ()
{ crn_lr . Set (frl_lr);  crn_ur . Set (frl_ur);
  crn_ul . Set (frl_ul);  crn_ll . Set (frl_ll);
}

void SonoChoosist::Unfurl ()
{ crn_lr . Set (unf_lr);  crn_ur . Set (unf_ur);
  crn_ul . Set (unf_ul);  crn_ll . Set (unf_ll);
}
