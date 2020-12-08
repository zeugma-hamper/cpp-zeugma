
#include "OeuvreAfterlife.h"

#include "ScGrappler.h"
#include "TrGrappler.h"


using namespace charm;


Ollag::Ollag (const std::string &fname)  :  Alignifer (),
                                            re (NULL), fr (NULL),
                                            cur_maes (NULL),
                                            own_private_over (Vect::xaxis),
                                            own_private_up (Vect::yaxis)
{ VideoSystem *vsys = VideoSystem::GetSystem ();

  const VideoBrace br = vsys -> OpenVideoFile (fname);
  re = new MattedVideoRenderable (br.video_texture);

  re -> SetEnableMatte (false);
  re -> SetSizeReferent (SizeReferent::Video);
  AppendRenderable (re);

  pi = br.control_pipeline;
  f64 dur = pi -> Duration ();
  // if (pi)
  //   pi -> Loop (0.0, pi -> Duration ());

  ScaleVect lat_slosh (own_private_over, lateral_sway);
  ScaleVect vrt_bobbl (own_private_up, vertical_bobble);
  aggregate_local_motion . SummandA () . BecomeLike (vrt_bobbl);
  aggregate_local_motion . SummandB () . BecomeLike (lat_slosh);
  SumVect totality (central_loc, aggregate_local_motion);
  LocZoft () . BecomeLike (central_loc); //totality);

  if (TrGrappler *trg
      = dynamic_cast <TrGrappler *> (FindGrappler ("loc")))
    trg -> TranslationZoft () . BecomeLike (LocZoft ());
}


void Ollag::AlignOverUp (const Vect &ov, const Vect &up)
{ Alignifer::AlignOverUp (ov, up);

  own_private_over . Set (ov);
  own_private_up . Set (up);
}


void Ollag::AlignToMaes (const PlatonicMaes *maes)
{ if (maes)
    cur_maes = maes;
  Alignifer::AlignToMaes (maes);
}


void Ollag::SetCentralLoc (const Vect &l)
{  central_loc . Set (l); }  // loc . Set (l); }


//
/// Ollag above; OeuvreAfter below
//


OeuvreAfterlife::OeuvreAfterlife ()  :  SpectacleCauseway ()
{ }


void OeuvreAfterlife::AppendCollage (Ollag *ol)
{ if (! ol)
    return;

//  ol -> ClearTransforms ();
//  ol -> Scale (Tamparams::Current ()->coll_scale);
  if (ScGrappler *scg
      = dynamic_cast <ScGrappler *> (ol -> FindGrappler ("scale")))
    scg -> ScaleZoft () . Set (Tamparams::Current ()->coll_scale);

  llages . push_back (ol);
  if (amok_field)
    amok_field -> AppendChild (ol);
}


void OeuvreAfterlife::DistributeCollagesEquitably ()
{ f64 l = MeanderLength ();
  f64 spcng = Tamparams::Current ()->coll_spacing;
  l -= 0.5 * spcng;

  for (auto it = llages . rbegin ()  ;  it != llages . rend ()  ;  ++it)
    { if (Ollag *ag = *it)
        { auto maesandpos = MaesAndPosFromMeanderDist (l);
          ag -> AlignToMaes (maesandpos.first);
          ag -> SetCentralLoc (maesandpos.second);
        }
      l -= spcng;
    }
}


i64 OeuvreAfterlife::Inhale (i64 ratch, f64 thyme)
{
  return 0;
}
