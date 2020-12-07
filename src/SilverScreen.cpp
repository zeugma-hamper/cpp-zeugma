
#include "SilverScreen.h"

#include <VideoSystem.hpp>

#include <vector_interop.hpp>


SilverScreen::SilverScreen (MattedVideoRenderable *vr,
                            ch_ptr <VideoPipeline> vp,
                            const FilmInfo &fi)
 :  Node (), finf (fi), vren (vr), vpip (vp), timmy (NULL)
{ frtr = new RectRenderableFrontier (vr, Vect::zerov, 1.0, 1.0);
  SetFrontier (frtr);
  scr_fader . SetInterpTime (Tamparams::Current ()->pb_snapback_fade_time);
  scr_fader . PointA () . Set (ZeColor (1.0, 1.0, 1.0, 0.0));
  scr_fader . PointB () . Set (ZeColor (1.0));
  scr_fader . Finish ();
  scr_fader . MakeBecomeLikable ();
  vren -> AdjColorZoft () . BecomeLike (scr_fader);
}


f64 SilverScreen::AspectRatio ()
{ if (! vren)
    return -1.0;

  v2i32 dimmy = vren -> GetVideoDimensions ();
  return (f64)dimmy.y / (f64)dimmy.x;
}


Vect SilverScreen::WoCoCenter ()
{ Matrix44 emm = from_glm (GetAbsoluteTransformation ().model);
  return emm . TransformVect (Vect::zerov);
}


std::pair <f64, f64> SilverScreen::WoCoWidHei ()
{ Matrix44 emm = from_glm (GetAbsoluteTransformation ().model);
  Vect widph
    = emm . TransformVect (Vect::xaxis)  -  emm . TransformVect (Vect::zerov);
  f64 wid = widph . Mag ();
  return std::pair (wid, AspectRatio () * wid);
}


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


bool SilverScreen::IsPaused ()
{ if (! vren)
    return true;
  ch_ptr <VideoPipeline> veep = vren -> GetVideoPipeline ();
  if (! veep)
    return true;   // though really maybe something worse?

  MediaStatus mest = veep -> GetStatus ();
  return (mest  ==  MediaStatus::Paused);
}

void SilverScreen::Pause (bool including_post_pause_rigamarole)
{ if (! vren)  return;
  ch_ptr <VideoPipeline> veep = vren -> GetVideoPipeline ();
  if (veep)
    { veep -> Pause ();
      if (including_post_pause_rigamarole)
        { f64 ts = veep -> CurrentTimestamp ();
          nascent_atoms = finf . ClipsSurrounding (ts);
          ReckonNatomBboxes ();
        }
    }
}

void SilverScreen::Play ()
{ if (! vren)  return;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (deep)
    deep -> Play ();
  ClearNascentAtoms ();
}

void SilverScreen::TogglePlayPause ()
{ if (! vren)  return;
  ch_ptr <VideoPipeline> veep = vren -> GetVideoPipeline ();
  if (! veep)  return;
  MediaStatus mest = veep -> GetStatus ();
  if (mest  ==  MediaStatus::Paused)
    Play ();
  else if (mest  ==  MediaStatus::Playing)
    Pause ();
}


bool SilverScreen::StepBy (i32 num_frames)
{ if (! vren)
    return false;
  ch_ptr <VideoPipeline> veep = vren -> GetVideoPipeline ();
  if (! veep)
    return false;

  veep -> Pause ();
  veep -> Step (num_frames);
  Pause ();  // ours, here in this very same class.
  return true;
}


f64 SilverScreen::Duration ()
{ if (! vren)  return -1.0;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (deep)
    return deep -> Duration ();
  return -1.0;
}


f64 SilverScreen::CurTimestamp ()
{ if (! vren)  return -1.0;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (deep)
    return deep -> CurrentTimestamp ();
  return -1.0;
}


const ClipInfo *SilverScreen::NthNascentAtom (i64 ind)
{ if (ind < 0  ||  ind >= nascent_atoms . size ())
    return NULL;
  return nascent_atoms . at (ind);
}

WoCoLoBbox *SilverScreen::NthNatomBbox (i64 ind)
{ if (ind < 0  ||  ind >= nascent_atoms . size ())
    return NULL;
  return &(natom_bboxes . at (ind));
}


WoCoLoBbox *SilverScreen::NatomBboxByClip (const ClipInfo *clinf)
{ if (clinf)
    for (WoCoLoBbox &bb  :  natom_bboxes)
      if (bb.clnf  ==  clinf)
        return &bb;
  return NULL;
}

void SilverScreen::ReckonNatomBboxes ()
{ if (! vren)
    assert (&"black"  ==  &"white");
  natom_bboxes . clear ();
  v2i32 vid_wh = vren -> GetVideoDimensions ();
  f64 asp_rat = (f64)vid_wh.y / (f64)vid_wh.x;
  i64 num = NumNascentAtoms ();
  Matrix44 emm = from_glm (GetAbsoluteTransformation ().model);
  for (i64 q = 0  ;  q < num  ;  ++q)
    if (const ClipInfo *clip = NthNascentAtom (q))
      { const MatteGeometry &mg = clip -> Geometry ();
        Vect clip_cnt
          (0.5 * ((f64)mg.min.x + (f64)mg.max.x) / (f64)vid_wh.x  -  0.5,
           asp_rat
            * (0.5  -  0.5 * ((f64)mg.min.y + (f64)mg.max.y) / (f64)vid_wh.y),
           0.0);
        Vect widph ((f64)mg.dimensions.x / (f64)vid_wh.x, 0.0, 0.0);
        Vect heiph (0.0, asp_rat * (f64)mg.dimensions.y / (f64)vid_wh.y, 0.0);
        Vect zee;
        emm . TransformVectInPlace (clip_cnt);
        emm . TransformVectInPlace (widph);
        emm . TransformVectInPlace (heiph);
        emm . TransformVectInPlace (zee);
        widph -= zee;  widph *= 0.5;
        heiph -= zee;  heiph *= 0.5;
        natom_bboxes . push_back ( {clip,
                                    clip_cnt,
                                    2.0 * widph . Mag (),
                                    2.0 * heiph . Mag (),
                                    clip_cnt + widph - heiph,
                                    clip_cnt + widph + heiph,
                                    clip_cnt - widph + heiph,
                                    clip_cnt - widph - heiph} );
      }
    else
      assert (true == false);   // that'd be bad enough we'd want to know...
}

bool SilverScreen::JumpToTime (f64 tstamp)
{ if (! vren)
    return false;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (! deep)
    return false;
  deep -> Seek (tstamp);
  ClearNascentAtoms ();
  return true;
}


bool SilverScreen::ScootToTime (f64 tstamp)
{ if (! vren  ||  ! vpip)
    return false;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (! deep)
    return false;
  if (tstamp < 0.0  ||  tstamp > deep -> Duration ())
    return false;

  f64 curt = CurTimestamp ();
  f64 tdist = fabs (tstamp - curt);

  deep -> TrickSeekTo (tstamp, 1.25);  // scootrate);
  ClearNascentAtoms ();

  return true;
}

bool SilverScreen::ScootToNextClip ()
{ f64 ts = CurTimestamp ();
  const ClipInfo *cl = finf . FirstClipBeginningAfter (ts);
  return cl  ?  ScootToTime (cl->start_time)  :  false;
}

bool SilverScreen::ScootToPrevClip ()
{ f64 ts = CurTimestamp ();
  const ClipInfo *cl = finf . FirstClipEndingBefore (ts);
  return cl  ?  ScootToTime (cl->end_time - (1.0/24.0))  :  false;
}


void SilverScreen::AttachTimeline (Timeline *tl)
{ if (! (timmy = tl))
    return;

  timmy -> SetRepDuration (Duration ());
  timmy -> SetPlayTime (CurTimestamp ());
}


void SilverScreen::DetachTimeline (Timeline *tl)
{ timmy = NULL; }
