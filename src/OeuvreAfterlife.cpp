
#include "OeuvreAfterlife.h"

#include "ScGrappler.h"
#include "TrGrappler.h"

#include "tamparams.h"


using namespace charm;


Ollag::Ollag (const std::string &fname)  :  Alignifer (), re (NULL),
                                            fr (NULL), cur_maes (NULL),
                                            own_private_idahover (Vect::xaxis),
                                            own_private_idahup (Vect::yaxis)
{ VideoSystem *vsys = VideoSystem::GetSystem ();

  const VideoBrace br = vsys -> OpenVideoFile (fname);
  re = new MattedVideoRenderable (br.video_texture);

  re -> SetEnableMatte (false);
  re -> SetSizeReferent (SizeReferent::Video);
  AppendRenderable (re);

  pi = br.control_pipeline;
  if (pi)
    { f64 dur = pi -> Duration ();
      pi -> Loop (0.0, pi -> Duration () - 0.04 * dur * drand48 ());
//      pi -> Seek (dur * drand48 ());
    }

  central_loc . MakeBecomeLikable ();

  ScaleVect lat_slosh (own_private_idahover, lateral_sway);
  ScaleVect vrt_bobbl (own_private_idahup, vertical_bobble);
  aggregate_local_motion . SummandA () . BecomeLike (vrt_bobbl);
  aggregate_local_motion . SummandB () . BecomeLike (lat_slosh);
  SumVect totality (central_loc, aggregate_local_motion);
  LocZoft () . BecomeLike (totality);

  if (TrGrappler *trg
      = dynamic_cast <TrGrappler *> (FindGrappler ("loc")))
    trg -> TranslationZoft () . BecomeLike (LocZoft ());
}


void Ollag::AlignOverUp (const Vect &ov, const Vect &up)
{ Alignifer::AlignOverUp (ov, up);

  own_private_idahover . Set (ov);
  own_private_idahup . Set (up);
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

  f64 s = Tamparams::Current ()->coll_scale;

  if (ScGrappler *scg
      = dynamic_cast <ScGrappler *> (ol -> FindGrappler ("scale")))
    scg -> ScaleZoft () . Set (s);

  ol->vertical_bobble . Frequency () . Set (0.035 + 0.035 * drand48 ());
  ol->vertical_bobble . Amplitude () . Set (s * 0.0425 * (1.0 + drand48 ()));

  ol->lateral_sway . Frequency () . Set (0.025 + 0.025 * drand48 ());
  ol->lateral_sway . Amplitude () . Set (s * 0.0374 * (1.0 + drand48 ()));

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
