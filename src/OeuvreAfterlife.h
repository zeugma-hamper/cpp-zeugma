
#ifndef OEUVRE_AFTERLIFE_REQUIRES_SURPRISINGLY_FEW_GOOD_DEEDS_TO_ATTAIN
#define OEUVRE_AFTERLIFE_REQUIRES_SURPRISINGLY_FEW_GOOD_DEEDS_TO_ATTAIN


#include "SpectacleCauseway.h"

#include "Alignifer.h"

#include "MattedVideoRenderable.hpp"
#include "Frontier.hpp"

#include "SumZoft.h"
#include "ScaleZoft.h"
#include "SinuZoft.h"
#include "InterpZoft.h"

#include <vector>


namespace charm  {


class Ollag  :  public Alignifer
{ public:
  MattedVideoRenderable *re;
  ch_ptr <VideoPipeline> pi;
  RectRenderableFrontier *fr;
  const PlatonicMaes *cur_maes;
  SumVect aggregate_local_motion;
  SinuFloat vertical_bobble;
  SinuFloat lateral_sway;
  ZoftVect own_private_idahover;
  ZoftVect own_private_idahup;
  ZoftVect central_loc;
  InterpFloat conga_pos;
  i64 conga_ordinal;
  i64 conga_directn;
  MotherTime conga_timer;

  Ollag (const std::string &fname);

  void AlignOverUp (const Vect &ov, const Vect &up)  override;
  void AlignToMaes (const PlatonicMaes *maes)  override;

  bool PresentlyCongaing ()
    { return ! conga_pos . Replete (); }

  void SetCentralLoc (const Vect &l);
};


class OeuvreAfterlife  :  public SpectacleCauseway
{ public:
  std::vector <Ollag *> llages;

  OeuvreAfterlife ();

  void AppendCollage (Ollag *ol);
  void DistributeCollagesEquitably ();

  Ollag *CollageFromOrdinal (i64 ord);

  void IntroduceNewCollage (Ollag *nol);

  void OverseeCongaAction (Ollag *ag);

  i64 Inhale (i64 ratch, f64 thyme)  override;
};


}


#endif
