
#ifndef SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE
#define SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE


#include "Alignifer.h"

#include "PolygonRenderable.h"
#include "TexturedRenderable.hpp"

#include "InterpZoft.h"


using namespace charm;


struct Choizl  :  public Node
{ TexturedRenderable *texre;
  InterpVect perky_loc;
  InterpVect mopey_sca;

  Choizl ();
};


class SonoChoosist  :  public Alignifer
{ public:

  SonoChoosist (const PlatonicMaes *maes);

  PolygonRenderable *brdr_re;
  std::vector <Choizl *> choizls;
  std::vector <Choizl *> in_storage;
  f64 wid, hei;
  f64 brd_thc;
  f64 chz_dia;
  InterpVect crn_lr, crn_ur, crn_ul, crn_ll;
  Vect frl_lr, frl_ur, frl_ul, frl_ll;
  Vect unf_lr, unf_ur, unf_ul, unf_ll;

  i64 NumChoizls ()  const
    { return choizls . size (); }
  void SetNumChoizls (i64 nc);

  void Furl ();
  void Unfurl ();
};



#endif
