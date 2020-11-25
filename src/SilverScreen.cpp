
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


