
#ifndef ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY
#define ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY


#include "Ticato.h"

#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"

#include "GeomFumble.h"

#include <vector>
#include <map>


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


class AtomicFreezone  :  public Zeubject,
                         public ZESpatialPhagy,
                         public ZEYowlPhagy
{ public:
  std::vector <FilmInfo> *cineganz;
  std::vector <Ticato *> atoms;
  Node *field_amok;
  f64 atom_count_goal;
  f64 inter_arrival_t;
  f64 max_speed, min_speed;
  f64 prev_time;

  std::map <std::string, Ticato *> hoverees;
  std::map <std::string, Ticato *> yankees;

  std::vector <Swath *> meander;
  f64 meander_len;
  G::Plane over_lid, undr_lid;

  void AppendSwath (Swath *sw);
  Swath *SwathFor (PlatonicMaes *ma);

  Ticato *FirstHitAtom (const G::Ray &ra, Vect *hitp);

  Ticato *AtomHoveredBy (const std::string &prv)
    { auto it = hoverees . find (prv);
      return (it == hoverees . end ())  ?  NULL  :  it->second;
    }
  Ticato *AtomYankedBy (const std::string &prv)
    { auto it = yankees . find (prv);
      return (it == yankees . end ())  ?  NULL  :  it->second;
    }

  void DetachAndDisposeOfAtom (Ticato *icat);
  Ticato *InstanitateAtom (const Vect &loc, PlatonicMaes *mae, i32 direc = 0);
  void SpontaneouslyGenerateAtomAtBoundary ();
  void PopulateFromScratch ();

  void PerambulizeAtoms (f64 dt);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;

  i64 Inhale (i64 ratchet, f64 thyme)  override;
};


#endif
