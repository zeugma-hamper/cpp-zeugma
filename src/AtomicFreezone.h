
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
  Swath *nxt, *prv;
  Swath ()  :  supporting_maes (NULL), nxt (NULL), prv (NULL)  { }
  Swath (const G::Segment &ro, const G::Segment &lu, PlatonicMaes *ma)
     :  prone (ro), plumb (lu), supporting_maes (ma), nxt (NULL), prv (NULL)
    { }
};


class AtomicFreezone  :  public Zeubject
{ public:
  std::vector <FilmInfo> *cineganz;
  std::vector <Ticato *> atoms;
  Node *field_amok;
  f64 atom_count_goal;
  f64 inter_arrival_t;
  f64 max_speed, min_speed;
  f64 prev_time;

  std::vector <Swath *> meander;
  f64 meander_len;
  G::Plane over_lid, undr_lid;

  void AppendSwath (Swath *sw);
  Swath *SwathFor (PlatonicMaes *ma);

  Ticato *InstanitateAtom (const Vect &loc, PlatonicMaes *mae, i32 direc = 0);
  void SpontaneouslyGenerateAtomAtBoundary ();
  void PopulateFromScratch ();

  void PerambulizeAtoms (f64 dt);

  i64 Inhale (i64 ratchet, f64 thyme)  override;
};


#endif
