
#include "GraumanPalace.h"

#include <VideoSystem.hpp>


GraumanPalace::GraumanPalace ()  :  Zeubject (), Node (), backing_maes (NULL)
{ }


void GraumanPalace::ImportExhibitionRoster (const std::vector <FilmInfo> &fimmz)
{ u32 cnt = fimmz . size ();
  for (u32 q = 0  ;  q < cnt  ;  ++q)
    { const FilmInfo &finf = fimmz[q];
      fprintf (stderr, "oh yes: about to load <%s>\n", finf.name . c_str ());

      VideoRenderable *vire = new VideoRenderable (finf);
      if (! vire)
        continue;  // is this even possible? doubt it; but just in case...
      movies . push_back (vire);

      SilverScreen *sisc = new SilverScreen (finf);
      screens . push_back (sisc);
      sisc -> AppendRenderable (vire);
/*
      ch_ptr <DecodePipeline> depi = vire -> GetPipeline ();
      f64 dur = depi -> Duration ();
      depi -> Seek (0.314159265359 * dur);
      depi -> Pause ();
*/
      AppendChild (sisc);
sisc -> Scale (500.0);
sisc -> Translate (Vect ((q-7) * 560.0, 0.0, 0.0));
    }
}
