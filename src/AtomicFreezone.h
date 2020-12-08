
#ifndef ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY
#define ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY


#include "Ticato.h"

#include "SpectacleCauseway.h"

#include <map>


using namespace charm;


using AtomInfoPair = std::pair <const FilmInfo *, const ClipInfo *>;


class AtomicFreezone  :  public SpectacleCauseway
{
 public:
  std::vector <FilmInfo> *cineganz;
  std::vector <Ticato *> atoms;
  Node *field_amok;
  f64 atom_count_goal;
  f64 inter_arrival_t;
  f64 max_speed, min_speed;
  f64 prev_time;
  std::vector <AtomInfoPair> privileged_atoms;
  f64 privileged_atom_probability;

  std::map <std::string, Ticato *> hoverees;
  std::map <std::string, Ticato *> yankees;

  f64 meander_len;
  G::Plane over_lid, undr_lid;

  AtomicFreezone ();

  AtomInfoPair ClipFromUniqueAtomName (const std::string &uq_nm);

  bool AppendPrivilegedAtom (const std::string &canon_name);
  void ClearPrivilegedAtoms ()
    { privileged_atoms . clear (); }

  f64 PrivilegedAtomProbability ()  const
    { return privileged_atom_probability; }
  void SetPrivlegedAtomProbability (f64 prob)
    { privileged_atom_probability = prob; }

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
//  Ticato *ReceiveAtomGraciously (Ticato *tic);
  void SpontaneouslyGenerateAtomAtBoundary ();
  void PopulateFromScratch ();

  void PerambulizeAtoms (f64 dt);

  i64 ZESpatialMove (ZESpatialMoveEvent *e)  override;
  i64 ZESpatialHarden (ZESpatialHardenEvent *e)  override;
  i64 ZESpatialSoften (ZESpatialSoftenEvent *e)  override;

  i64 ZEBulletin (ZEBulletinEvent *e)  override;

  i64 Inhale (i64 ratchet, f64 thyme)  override;
};


#endif
