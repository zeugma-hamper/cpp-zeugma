
#ifndef SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE
#define SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE


#include "Alignifer.h"

#include "PolygonRenderable.h"

#include "InterpZoft.h"


using namespace charm;


class SonoChoosist  :  public Alignifer
{ public:

  SonoChoosist (PlatonicMaes *maes);

  PolygonRenderable *brdr_re;
  f64 wid, hei;
  f64 brd_thc;
  InterpVect crn_lr, crn_ur, crn_ul, crn_ll;
  Vect frl_lr, frl_ur, frl_ul, frl_ll;
  Vect unf_lr, unf_ur, unf_ul, unf_ll;

  void Furl ();
  void Unfurl ();
};



#endif
