
#include "OeuvreAfterlife.h"

#include "ScGrappler.h"
#include "TrGrappler.h"

#include "tamparams.h"


using namespace charm;


Ollag::Ollag (const std::string &fname)  :  Alignifer (), re (NULL),
                                            fr (NULL), cur_maes (NULL),
                                            own_private_idahover (Vect::xaxis),
                                            own_private_idahup (Vect::yaxis)
{ if (! fname . empty ())
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
    }

  central_loc . MakeBecomeLikable ();

  ScaleVect lat_slosh (own_private_idahover, lateral_sway);
  ScaleVect vrt_bobbl (own_private_idahup, vertical_bobble);
  aggregate_local_motion . SummandA () . BecomeLike (vrt_bobbl);
  aggregate_local_motion . SummandB () . BecomeLike (lat_slosh);
  SumVect totality (central_loc, aggregate_local_motion);

  InstallLocGrapplerZoft (totality);

  conga_pos . SetInterpFunc (InterpFuncs::QUADRATIC_AB);
  conga_timer . SetTimeFlowRate (-1.0);
  conga_timer . SetTime (-10.0);
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
          ag->conga_pos . SetHard (l);
        }
      l -= spcng;
    }
}



Ollag *OeuvreAfterlife::CollageFromOrdinal (i64 ord)
{ for (Ollag *ag  :  llages)
    if (ag)
      if (ag->conga_ordinal  ==  ord)
        return ag;
  return NULL;
}


void OeuvreAfterlife::IntroduceNewCollage (Ollag *nol)
{ if (! nol)
    return;

  nol->conga_ordinal = 0;

  if (Ollag *zerollage = CollageFromOrdinal (0))
    { i64 dir = zerollage->conga_directn;
      nol->conga_directn = -dir;
      nol -> AlignToOther (zerollage);
      nol -> SetCentralLoc (zerollage->central_loc.val);
      nol->conga_pos . SetHard (zerollage->conga_pos.val);

      for (Ollag *ag  :  llages)
        if (ag)
          if (dir * ag->conga_ordinal  >=  0)
            ag->conga_ordinal += dir;

      f64 sp = zerollage->conga_pos . PointB () . Val ();
      sp += (f64)dir * Tamparams::Current ()->coll_spacing;
      f64 slosht = Tamparams::Current ()->coll_slosh_time;
      zerollage->conga_pos . SetInterpTime (slosht);
      zerollage->conga_pos . Set (sp);
      zerollage->conga_timer . SetTime (-0.225 * slosht);
      zerollage->conga_timer . SetTimeFlowRate (1.0);
    }
  else
    nol->conga_directn = 1;

  AppendCollage (nol);
  // llages . push_back (nol);
  // if (amok_field)
  //   amok_field -> AppendChild (nol);
}


void OeuvreAfterlife::OverseeCongaAction (Ollag *ag)
{ if (! ag)
    return;

  f64 linpos = ag->conga_pos.val;
  auto maesandpos = MaesAndPosFromMeanderDist (linpos, false);
  ag -> AlignToMaes (maesandpos.first);
  ag -> SetCentralLoc (maesandpos.second);

  i64 dir = ag->conga_directn;
  if (ag->conga_timer . CurTimeGlance ()  >=  0)
    { f64 slosht = Tamparams::Current ()->coll_slosh_time;
      if (Ollag *nxtag = CollageFromOrdinal (ag->conga_ordinal + dir))
        { f64 sp = nxtag->conga_pos . PointB () . Val ();
          sp += (f64)dir * Tamparams::Current ()->coll_spacing;
          nxtag->conga_pos . SetInterpTime (slosht);
          nxtag->conga_pos . Set (sp);
          nxtag->conga_timer . SetTime (-(0.205 + 0.05 * drand48 ()) * slosht);
          nxtag->conga_timer . SetTimeFlowRate (1.0);
        }
      ag->conga_timer . SetTime (-10.0 * slosht);
      ag->conga_timer . SetTimeFlowRate (-1.0);
    }
}


i64 OeuvreAfterlife::Inhale (i64 ratch, f64 thyme)
{ for (Ollag *ag  :  llages)
    if (ag  &&  ag -> PresentlyCongaing ())
      OverseeCongaAction (ag);

  return 0;
}
