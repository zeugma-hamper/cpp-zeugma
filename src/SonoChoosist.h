
#ifndef SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE
#define SONO_CHOOSIST_IS_A_LONG_BUFFET_WITH_ACOUSTIC_FRICASSEE


#include "Alignifer.h"

#include "PolygonRenderable.h"
#include "TexturedRenderable.hpp"

#include "InterpZoft.h"

#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"
#include "ZEBulletinEvent.h"
#include "Ticato.h"
#include "Jigglegon.h"


using namespace charm;


struct Choizl  :  public Node
{ TexturedRenderable *texre;
  InterpVect perky_loc;
  InterpVect mopey_sca;
  i64 index;

  Choizl ();
};


class SonoChoosist  :  public Alignifer,
                       public ZESpatialPhagy, public ZEYowlPhagy,
                       public ZEBulletinPhagy
{ public:

  SonoChoosist (const PlatonicMaes *maes);

  PolygonRenderable *brdr_re;
  Node *chz_node;
  std::vector <Choizl *> choizls;
  std::vector <Choizl *> in_storage;
  Jigglegon *hexajig;
  Ticato *behalf_of;
  InterpFloat active;
  f64 contact_dist;
  std::unordered_map <std::string, Choizl *> hover;
  std::unordered_map <std::string, Choizl *> smack;
  f64 wid, hei;
  f64 brd_thc;
  f64 chz_dia;
  InterpVect crn_lr, crn_ur, crn_ul, crn_ll;
  Vect frl_lr, frl_ur, frl_ul, frl_ll;
  Vect unf_lr, unf_ur, unf_ul, unf_ll;

  i64 NumChoizls ()  const
    { return choizls . size (); }
  void PopulateChoizls (i64 nc);

  Vect Centerdom ()  const
    { return 0.25 * (crn_lr.val + crn_ur.val + crn_ul.val + crn_ll.val); }

  void InitiateAtomicContact (Ticato *tic);

  bool Active ()
    { return (active.val  >=  1.0); }
  f64 Activeishness ()
    { return active.val; }

  void Furl ();
  void Unfurl ();

  bool PointInAirspaceOver (const Vect &p,
                            Vect *hit_out = NULL, Matrix44 *mat_out = NULL);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;
};



#endif
