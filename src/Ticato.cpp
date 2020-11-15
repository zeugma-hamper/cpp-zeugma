
#include "Ticato.h"

#include "MattedVideoRenderable.hpp"


i64 ze_rand (i64 h, i64 l = 0)
{ return -l + i64(floorf64(h - l) * drand48 ()); }


Ticato::Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm, i64 which_clip)
  :  Zeubject (), no (NULL), re (NULL),
     sca (Vect (1.0, 1.0, 1.0)), cur_maes (NULL)
{ if (which_fimm  <  0)
    which_fimm = ze_rand (fimmz . size ());

  const FilmInfo &finf = fimmz[which_fimm];

  if (which_clip  <  0)
    which_clip = ze_rand (finf.clips . size ());
  assert (which_clip  <  finf.clips . size ());

  const ClipInfo &clinf = finf.clips[which_clip];

MattedVideoRenderable *curre, *prere = NULL;
for (int q = 0  ;  q < 1  ;  ++q) {                  // <---- adjust loop count
  curre = new MattedVideoRenderable (finf, clinf);   // to witness mem-devour;
if (prere)  delete prere;                            // '3' on the hades cyn
prere = curre;                                       // blows out memory and
} re = curre;                                        // dies in seconds

  (no = new Node) -> AppendRenderable (re);
  sca . MakeBecomeLikable ();
  loc . MakeBecomeLikable ();
  no -> Scale (sca);
  no -> Translate (loc);
}
