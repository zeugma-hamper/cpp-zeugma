
#ifndef ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY
#define ATOMIC_FREEZONE_IS_A_BLISSFUL_ANARCHY


#include "Ticato.h"

#include "GeomFumble.h"

#include <vector>


using namespace charm;


class AtomicFreezone  :  public Zeubject
{ public:
  std::vector <Ticato *> atoms;

  G::Plane over_lid, undr_lid;

  i64 Inhale (i64 ratchet, f64 thyme)  override;
};


#endif
