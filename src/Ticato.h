
#ifndef TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM
#define TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM


#include "Node.hpp"
#include "Renderable.hpp"
#include "Frontier.hpp"

#include "Matte.hpp"
#include "MattedVideoRenderable.hpp"

#include "Alignifer.h"
#include "MotherTime.h"

#include "PlatonicMaes.h"

#include "LoopZoft.h"

#include "tamparams.h"

#include <string>


using namespace charm;


enum class AtomThusness {
  CONT, STOP, NUKE
};

class Ticato  :  public Alignifer
{ public:
  const ClipInfo *atom_info;
  MattedVideoRenderable *re;
  RectRenderableFrontier *fr;
  Alignifer *bbox_subno;
  bool born_from_flick;  // else spontaneously generated

  ZoftVect wander_vel;
  ZoftVect accom_sca;
  f64 atom_dur;
  PlatonicMaes *cur_maes;

  stringy_list sono_options;
  i32 playing_sono;
  i64 playing_prfm_id;

  std::string hvrr;
  std::string ynkr;
  Vect shov_vel;
  Node *from_node;
  LoopFloat shabby_loop_minder;

  void WellAndTrulyConstruct (const FilmInfo &finf, const ClipInfo &clinf);

  Ticato ();
  Ticato (const FilmInfo &finf, const ClipInfo &clinf);
  Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm = -1,
          i64 which_clip = -1);

  ~Ticato ()  override;

  void ProvisionVisibleBounds ();

  const std::string &AtomName ()  const
    { static std::string empty_s;
      if (atom_info)
        return atom_info->geometry.niq_atomname;
      return empty_s;
    }

  bool BornViaFlickPluck ()  const
    { return born_from_flick; }
  bool BornViaSpontaneousGeneration ()  const
    { return ! born_from_flick; }

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
    { if (! ma)
        return;
       cur_maes = ma;
       AlignToMaes ();
       if (bbox_subno)
         bbox_subno -> AlignToMaes (ma);
    }

  const std::string CurHoverer ()  const
    { return hvrr; }
  bool BeHoveredBy (const std::string &prov);
  bool BeNotHoveredBy (const std::string &prov);

  const std::string CurYanker ()  const
    { return ynkr; }
  bool BeYankedBy (const std::string &prov);
  bool BeNotYankedBy (const std::string &prov);

  void BBoxSetColor (const ZeColor &iro);
  void BBoxSetColor (const ZoftColor &zc);

  bool SonoPlay ();
  bool SonoSilence ();
  bool EnunciateNthSonoOption (i64 ind);

  i64 Inhale (i64 ratch, f64 thyme);
};


#endif
