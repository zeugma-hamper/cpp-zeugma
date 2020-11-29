
#include "Ticato.h"

#include "ScGrappler.h"

#include "AudioMessenger.hpp"

#include "ZeUUID.h"


i64 ze_rand (i64 h, i64 l = 0)
{ return -l + i64(floorf64(h - l) * drand48 ()); }


Ticato::Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm, i64 which_clip)
  :  Alignifer (),
     atom_info (NULL), re (NULL), fr (NULL), accom_sca (1.0), atom_dur (-1.0),
     cur_maes (NULL), playing_sono (-1), playing_prfm_id (-1), from_node (NULL)
{ if (which_fimm  <  0)
    which_fimm = ze_rand (fimmz . size ());

  const FilmInfo &finf = fimmz[which_fimm];

  if (which_clip  <  0)
    which_clip = ze_rand (finf.clips . size ());
  assert (which_clip  <  finf.clips . size ());

  const ClipInfo &clinf = finf.clips[which_clip];
fprintf(stderr,"INSTANTIATING [%s]...\n",clinf.geometry.niq_atomname.c_str());
  re = new MattedVideoRenderable (finf, clinf);
  fr = new RectRenderableFrontier (re, Vect::zerov, 1.0, 1.0);
  AppendRenderable (re);
  SetFrontier (fr);
  atom_info = &clinf;

  accom_sca . MakeBecomeLikable ();
  if (GrapplerPile *gp = UnsecuredGrapplerPile ())
    { i64 ind = gp -> IndexForGrappler (gp -> FindGrappler ("loc"));
      ScGrappler *scg = new ScGrappler (accom_sca);
      scg -> SetName ("accom-scale");
      if (ind  >=  0)
        gp -> InsertGrappler (scg, ind);
      else
        gp -> AppendGrappler (scg);
    }

  atom_dur = clinf.duration;
  if (IronLung *irlu = IronLung::GlobalByName ("omni-lung"))
    irlu -> AppendBreathee (this);
}


Ticato::Ticato (const FilmInfo &finf, const ClipInfo &clinf)
  :  Alignifer (),
     atom_info (&clinf), re (NULL), fr (NULL), accom_sca (1.0), atom_dur (-1.0),
     cur_maes (NULL), playing_sono (-1), playing_prfm_id (-1), from_node (NULL)
{ // no ambiguity about it...
 fprintf(stderr,"INSTANTIATING [%s]...\n",clinf.geometry.niq_atomname.c_str());
  re = new MattedVideoRenderable (finf, clinf);
  fr = new RectRenderableFrontier (re, Vect::zerov, 1.0, 1.0);
  AppendRenderable (re);
  SetFrontier (fr);
  atom_info = &clinf;

  accom_sca . MakeBecomeLikable ();
  if (GrapplerPile *gp = UnsecuredGrapplerPile ())
    { i64 ind = gp -> IndexForGrappler (gp -> FindGrappler ("loc"));
      ScGrappler *scg = new ScGrappler (accom_sca);
      scg -> SetName ("accom-scale");
      if (ind  >=  0)
        gp -> InsertGrappler (scg, ind);
      else
        gp -> AppendGrappler (scg);
    }

  atom_dur = clinf.duration;
  if (IronLung *irlu = IronLung::GlobalByName ("omni-lung"))
    irlu -> AppendBreathee (this);
}


Ticato::~Ticato ()
{ if (IronLung *irlu = IronLung::GlobalByName ("omni-lung"))
    irlu -> RemoveBreathee (this);
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
  bool ret = (hvrr == prov);
  hvrr . clear ();
  return ret;
}

bool Ticato::BeYankedBy (const std::string &prov)
{ if (! ynkr . empty ())
    return false;
  ynkr = prov;
  return true;
}

bool Ticato::BeNotYankedBy (const std::string &prov)
{ if (prov . empty ())
    { if (ynkr . empty ())
        return false;
    }
  bool ret = (ynkr == prov);
  ynkr . clear ();
  return ret;
}


bool Ticato::SonoPlay ()
{ if (playing_sono >= 0  &&  playing_sono < sono_options . size ())
    if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
      { playing_prfm_id = ZeMonotonicID ();
        sherm -> SendPlaySound (sono_options[playing_sono], playing_prfm_id);
        shabby_loop_minder . Restart ();
        return true;
      }
  return false;
}

bool Ticato::SonoSilence ()
{ if (playing_prfm_id  <  0)
    return true;
  if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
    { sherm -> SendStopSound (playing_prfm_id);
      playing_prfm_id = -1;
      playing_sono = -1;
      return true;
    }
  return false;
}


bool Ticato::EnunciateNthSonoOption (i64 ind)
{ if (ind  ==  playing_sono)
    return true;
  if (ind  <  0)
    return SonoSilence ();
  if (ind  >=  sono_options . size ())
    return false;
  if (AudioMessenger *sherm = Tamglobals::Only ()->sono_hermes)
    { if (playing_prfm_id  >=  0)
        sherm -> SendStopSound (playing_prfm_id);
      playing_sono = ind;
      SonoPlay ();
      return true;
    }
  return false;
}


i64 Ticato::Inhale (i64 ratch, f64 thyme)
{ if (playing_sono >= 0  &&  atom_dur > 0)
    if (shabby_loop_minder.val  >  atom_dur)
      SonoPlay ();
  return 0;
}
