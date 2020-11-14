
#ifndef ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY
#define ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY


#include "Ticato.h"

#include "GeomFumble.h"

#include <vector>


using namespace charm;


struct Swath
{ G::Segment prone;
  G::Segment plumb;
  PlatonicMaes *supporting_maes;
};


class AtomicFreezone  :  public Zeubject
{ public:
  std::vector <FilmInfo> *cineganz;
  std::vector <Ticato *> atoms;
  Node *field_amok;
  f64 atom_count_goal;
  f64 inter_arrival_t;

  std::vector <Swath> meander;
  f64 meander_len;
  G::Plane over_lid, undr_lid;

  void PopulateFromScratch ();

  i64 Inhale (i64 ratchet, f64 thyme)  override;
};


#endif
