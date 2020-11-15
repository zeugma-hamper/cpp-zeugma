
#ifndef TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM
#define TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM


#include "Node.hpp"
#include "Renderable.hpp"

#include "Matte.hpp"

#include "Zeubject.h"
#include "MotherTime.h"

#include "PlatonicMaes.h"


using namespace charm;


class Ticato  :  public Zeubject
{ public:

  Node *no;
  Renderable *re;
  ZoftVect sca, loc;
  ZoftVect vel;
  PlatonicMaes *cur_maes;

  Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm = -1,
          i64 which_clip = -1);

  ~Ticato ()  override
    {
      fprintf (stderr, "Is <%p> really gonna jelly up the sidewalk?\n", this);
    }

  void AlignToMaes ()
    { if (cur_maes)
        { re -> SetOver (cur_maes -> Over ());
          re -> SetUp (cur_maes -> Up ());
        }
    }

  void SetAndAlignToMaes (PlatonicMaes *ma)
    { if (ma)
        { cur_maes = ma;  AlignToMaes (); }
    }
};


#endif
