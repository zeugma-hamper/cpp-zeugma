
#ifndef TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM
#define TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM


#include "Node.hpp"
#include "Renderable.hpp"
#include "Frontier.hpp"

#include "Matte.hpp"

#include "Alignifer.h"
#include "MotherTime.h"

#include "PlatonicMaes.h"

#include <string>


using namespace charm;


class Ticato  :  public Alignifer
{ public:
  const ClipInfo *atom_info;
  Renderable *re;
  RectRenderableFrontier *fr;
  ZoftVect vel;
  ZoftVect accom_sca;
  PlatonicMaes *cur_maes;
  std::string hvrr;
  std::string ynkr;
  Node *from_node;

  Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm = -1,
          i64 which_clip = -1);

  ~Ticato ()  override
    { }


  const std::string &AtomName ()  const
    { static std::string empty_s;
      if (atom_info)
        return atom_info->geometry.niq_atomname;
      return empty_s;
    }

  PlatonicMaes *CurMaes ()  const
    { return cur_maes; }
  void SetCurMaes (PlatonicMaes *ma)
    { cur_maes = ma; }

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

  const std::string CurHoverer ()  const
    { return hvrr; }
  bool BeHoveredBy (const std::string &prov);
  bool BeNotHoveredBy (const std::string &prov);

  const std::string CurYanker ()  const
    { return ynkr; }
  bool BeYankedBy (const std::string &prov);
  bool BeNotYankedBy (const std::string &prov);
};


#endif
