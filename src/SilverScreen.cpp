
#include "SilverScreen.h"

#include <VideoSystem.hpp>

#include <vector_interop.hpp>


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

void SilverScreen::TogglePlayPause ()
{ if (! vren)  return;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
  if (! deep)  return;
  DecodePipeline::MediaStatus mest = deep -> GetStatus ();
  if (mest  ==  DecodePipeline::MediaStatus::Paused)
    deep -> Play ();
  else if (mest  ==  DecodePipeline::MediaStatus::Playing)
    deep -> Pause ();
}


bool SilverScreen::StepBy (i32 num_frames)
{ if (! vren)
    return false;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
  if (! deep)
    return false;

  deep -> Pause ();
  deep -> Step (num_frames);
}


f64 SilverScreen::CurTimestamp ()
{ if (! vren)  return -1.0;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
  if (deep)
    return deep -> CurrentTimestamp ();
  return -1.0;
}


bool SilverScreen::ScootToTime (f64 tstamp)
{ if (! vren  ||  ! vpip)
    return false;
  ch_ptr <DecodePipeline> deep = vren -> GetPipeline ();
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

  deep -> TrickModeSeek (tstamp, 100.0);

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
  return cl  ?  ScootToTime (cl->end_time)  :  false;
}

