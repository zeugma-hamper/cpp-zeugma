
#include "Ticato.h"

#include "MattedVideoRenderable.hpp"


i64 ze_rand (i64 h, i64 l = 0)
{ return -l + i64(floorf64(h - l) * drand48 ()); }


Ticato::Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm, i64 which_clip)
  :  Zeubject (), no (NULL), re (NULL), fr (NULL),
     sca (Vect (1.0, 1.0, 1.0)), cur_maes (NULL)
{ if (which_fimm  <  0)
    which_fimm = ze_rand (fimmz . size ());

  const FilmInfo &finf = fimmz[which_fimm];

  if (which_clip  <  0)
    which_clip = ze_rand (finf.clips . size ());
  assert (which_clip  <  finf.clips . size ());

  const ClipInfo &clinf = finf.clips[which_clip];

  re = new MattedVideoRenderable (finf, clinf);
  fr = new RectRenderableFrontier (re, Vect::zerov, 1.0, 1.0);
  (no = new Node) -> AppendRenderable (re);
  no -> SetFrontier (fr);

  sca . MakeBecomeLikable ();
  loc . MakeBecomeLikable ();
  no -> Scale (sca);
  no -> Translate (loc);
}


bool Ticato::BeHoveredBy (const std::string &prov)
{ if (! hvrr . empty ())
    return false;
  hvrr = prov;
  return true;
}

bool Ticato::BeNotHoveredBy (const std::string &prov)
{ if (prov . empty ())
    { if (hvrr . empty ())
        return false;
    }
  if (hvrr  !=  prov)
    { hvrr . clear ();
      return false;
    }
  hvrr . clear ();
  return true;
}
