
#include "SilverScreen.h"

#include <VideoSystem.hpp>

#include <vector_interop.hpp>


f64 SilverScreen::AspectRatio ()
{ if (! vren)
    return -1.0;

  v2i32 dimmy = vren -> GetVideoDimensions ();
  return (f64)dimmy.y / (f64)dimmy.x;
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

void SilverScreen::Pause ()
{ if (! vren)  return;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (deep)
    deep -> Pause ();
}

void SilverScreen::Play ()
{ if (! vren)  return;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (deep)
    deep -> Play ();
}

void SilverScreen::TogglePlayPause ()
{ if (! vren)  return;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (! deep)  return;
  MediaStatus mest = deep -> GetStatus ();
  if (mest  ==  MediaStatus::Paused)
    deep -> Play ();
  else if (mest  ==  MediaStatus::Playing)
    deep -> Pause ();
}


bool SilverScreen::StepBy (i32 num_frames)
{ if (! vren)
    return false;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (! deep)
    return false;

  deep -> Pause ();
  deep -> Step (num_frames);
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


bool SilverScreen::JumpToTime (f64 tstamp)
{ if (! vren)
    return false;
  ch_ptr <VideoPipeline> deep = vren -> GetVideoPipeline ();
  if (! deep)
    return false;
  deep -> Seek (tstamp);
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

  // auto seg_done_cb = [deep, tstamp] (boost::signals2::connection conn,
  //                                    DecodePipeline *, SegmentDoneBehavior)
  //   { deep -> Seek (tstamp);
  //     conn . disconnect ();
  //   };
  // deep -> AddSegmentDoneExCallback (std::move (seg_done_cb));

  f64 curt = CurTimestamp ();
  f64 tdist = fabs (tstamp - curt);
//  f64 frate = deep -> CurrentVideoFrameRate ();
  f64 scootrate = tdist < 1.5  ?  tdist  :  tdist / 1.5;
  deep -> TrickSeekTo (tstamp, 1.75);  // scootrate);
//  deep -> ReadTheCommentBelowThis();
  //jh, the second parameter to TrickSeekTo is how long the duration should take
  //not the "play speed". this is, I think, easier for you and for me.

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
  //return cl  ?  ScootToTime (cl->start_time)  :  false;
}


void SilverScreen::AttachTimeline (Timeline *tl)
{ if (! (timmy = tl))
    return;

  timmy -> SetRepDuration (Duration ());
  timmy -> SetPlayTime (CurTimestamp ());
}


void SilverScreen::DetachTimeline (Timeline *tl)
{ timmy = NULL; }
