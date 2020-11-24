
#include "GraumanPalace.h"

#include <VideoSystem.hpp>

#include <vector_interop.hpp>


//
/// SilverScreen, just for a bit
//

void SilverScreen::FadeUp ()
{ scr_fader . PointA () . Set (ZeColor (1.0, 0.0));
  scr_fader . PointB () . Set (ZeColor (1.0, 1.0));
  scr_fader . Commence ();
}

void SilverScreen::FadeDown ()
{ scr_fader . PointA () . Set (ZeColor (1.0, 1.0));
  scr_fader . PointB () . Set (ZeColor (1.0, 0.0));
  scr_fader . Commence ();
}

void SilverScreen::Pause ()
{ if (! vren)  return;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
  if (deep)
    deep -> Pause ();
}

void SilverScreen::Play ()
{ if (! vren)  return;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
  if (deep)
    deep -> Play ();
}


//
///  GraumanPalace items of every stripe follow
//

GraumanPalace::GraumanPalace ()  :  Zeubject (), Node (),
                                    backing_maes (NULL),
                                    flick_wid (3600.0), flick_spacing (3840.0),
                                    ovr (Vect::xaxis), upp (Vect::yaxis),
                                    nrm (Vect::zaxis), slider (new Node),
                                    now_showing (0),
                                    pb_depth_scale (30.0), pb_slide_scale (20.0),
                                    pb_max_push (-50000.0), pb_max_pull (5000.0)
{ AppendChild (slider);

  push_depth . SetInterpTime (Tamparams::Current ()->pb_snapback_interp_time);
  ltrl_slide . SetInterpTime (Tamparams::Current ()->lateral_slide_interp_time);

  Grappler *g = slider -> Translate (ltrl_slide);
  g -> SetName ("slide-trans");
  g = slider -> Translate  (push_depth);
  g -> SetName ("push-trans");
}


SilverScreen *GraumanPalace::NthSilverScreen (i64 ind)
{ if (ind < 0  ||  ind >= screens . size ())
    return NULL;
  return screens[ind];
}

VideoRenderable *GraumanPalace::NthFlick (i64 ind)
{ if (SilverScreen *s = NthSilverScreen (ind))
    return s -> vren;
  return NULL;
}


void GraumanPalace::JumpToFlick (i64 which_flick)
{ i64 flick_cnt = screens . size ();
  if (flick_cnt  <  1)
    return;

  if (which_flick  <  0)
    which_flick = 0;
  else if (which_flick  >=  screens . size ())
    which_flick = screens . size () - 1;

  if (which_flick  ==  now_showing)
    return;

  if (SilverScreen *ss = NthSilverScreen (now_showing))
    ss -> Pause ();

  if (SilverScreen *ss = NthSilverScreen (which_flick))
    ss -> Play ();

  now_showing = which_flick;
  ltrl_slide . ProceedTo (-now_showing * flick_spacing * Over ());
  ltrl_slide . Commence ();
}


void GraumanPalace::TogglePlayPause ()
{ VideoRenderable *vire = CurFlick ();
  if (! vire)
    return;

  const ch_ptr <DecodePipeline> deep = vire -> GetPipeline ();
  if (! deep)
    assert (3 == 5);  // let's hope not.

  DecodePipeline::MediaStatus mest = deep -> GetStatus ();
  if (mest  ==  DecodePipeline::MediaStatus::Paused)
    deep -> Play ();
  else if (mest  ==  DecodePipeline::MediaStatus::Playing)
    deep -> Pause ();
}


void GraumanPalace::ImportExhibitionRoster (const std::vector <FilmInfo> &fimmz)
{ u32 cnt = fimmz . size ();
  for (u32 q = 0  ;  q < cnt  ;  ++q)
    { const FilmInfo &finf = fimmz[q];
      fprintf (stderr, "oh yes: about to load <%s>\n", finf.name . c_str ());

      VideoRenderable *vire = new VideoRenderable (finf);
      if (! vire)
        continue;  // is this even possible? doubt it; but just in case...
      vire -> SetOver (Over ());
      vire -> SetUp (Up ());
vire->SetAdjColor(ZeColor(1.0,1.0,0.1,1.0));
      SilverScreen *sisc = new SilverScreen (vire, finf);
      screens . push_back (sisc);
      sisc -> AppendRenderable (vire);

      slider -> AppendChild (sisc);
      sisc -> Scale (flick_wid);
      sisc -> Translate ((f64)q * flick_spacing * Over ());
    }
}


void GraumanPalace::EngagePushback ()
{ SilverScreen *curss = CurSilverScreen ();
  for (SilverScreen *ss  :  screens)
    if (ss  !=  curss)
      ss -> FadeUp ();
}

void GraumanPalace::ReleasePushback ()
{ // fade down all but the now-playing flick
  i64 closest_flick = i64 (0.5 - ovr . Dot (ltrl_slide.val) / flick_spacing);
  JumpToFlick (closest_flick);
  push_depth . ProceedTo (Vect::zerov);
  push_depth .  Commence ();

  SilverScreen *curss = CurSilverScreen ();
  for (SilverScreen *ss  :  screens)
    if (ss  !=  curss)
      { ss -> FadeDown ();
        ss -> Pause ();
      }
}


i64 GraumanPalace::ZESpatialMove (ZESpatialMoveEvent *e)
{ if (! e)
    return -1;

  if (e -> Provenance ()  ==  effecting_pushback)
    { Vect offset = e -> Loc ()  -  pb_estab_loc;
      f64 puba = pb_depth_scale * nrm . Dot (offset);
      f64 latr = -pb_estab_flick * flick_spacing
                  +  pb_slide_scale * ovr . Dot (offset);
      // impose translational limits here...
      if (puba > pb_max_pull)
        puba = pb_max_pull;
      else if (puba < pb_max_push)
        puba = pb_max_push;

      push_depth . ProceedTo (puba * nrm);
      ltrl_slide . ProceedTo (latr * ovr);
    }
  return 0;
}

i64 GraumanPalace::ZESpatialHarden (ZESpatialHardenEvent *e)
{ if (! e)
    return -1;
  if (! effecting_pushback . empty ())
    return 0;

  Vect eym = e -> Aim ();
  if (eym . Dot (upp)  >  0.8)  // pointing noticeably heavenward..
    { effecting_pushback = e -> Provenance ();
      pb_estab_loc = e -> Loc ();
      pb_estab_flick = now_showing;
      EngagePushback ();
    }
  else if (eym . Dot (ovr)  >  0.8)  // to the right!
    { TogglePlayPause (); }
  else if (SilverScreen *s = CurSilverScreen ())
    { Vect hit;
      if (s->vren
          &&  s->frtr -> CheckHit (G::Ray (e -> Loc (), e -> Aim ()), &hit))
         {fprintf (stderr, "hit CINEMA! namely <%s>! and at ",
                   s->finf.name . c_str ());
           hit . SpewToStderr ();  fprintf (stderr, "\n");
           Vect rig = 0.5 * s->vren -> Over ();
           Vect lef = -rig;
           Matrix44 m = from_glm (s -> GetAbsoluteTransformation ().model);
           m . TransformVectInPlace (lef);
           m . TransformVectInPlace (rig);
           f64 ww = rig . DistFrom (lef);
           f64 t = (hit - lef) . Dot ((rig - lef) . Norm ());
           if (ww  !=  0.0)
             t /= ww;
           fprintf (stderr, "HEY! HEY YOU! YEAH, YOU! WELL GUESS WHAT: TEE "
                    " = %.2lf\n", t);
           ch_ptr<DecodePipeline> depi = s->vren -> GetPipeline ();
           f64 dur = depi -> Duration ();
           if (t < 0.0)  t = 0.0;  else if (t > 1.0) t = 1.0;
           depi -> Seek (t * dur);
         }
    }
  return 0;
}

i64 GraumanPalace::ZESpatialSoften (ZESpatialSoftenEvent *e)
{ if (! e)
    return -1;
  if (e -> Provenance ()  !=  effecting_pushback)
    return 0;  // the phone's not for you

  ReleasePushback ();
  effecting_pushback . clear ();
  return 0;
}


i64 GraumanPalace::ZEYowlAppear (ZEYowlAppearEvent *e)
{ if (! e)
    return -1;
  const std::string &utt = e -> Utterance ();
  if (utt == "n")
    JumpToNextFlick ();
  else if (utt == "p")
    JumpToPrevFlick ();
  else if (utt == "z")
    { push_depth . ProceedTo (-15.0 * flick_wid * Norm ());
      push_depth .  Commence ();
      EngagePushback ();
    }
  else if (utt == "x")
    { ReleasePushback (); }
  else if (utt == " ")
    { TogglePlayPause (); }
  return 0;
}
