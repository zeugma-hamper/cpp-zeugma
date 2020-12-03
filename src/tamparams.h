
#ifndef TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM
#define TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM


#include "base_types.hpp"

#include "Node.hpp"
#include "InterpZoft.h"

#include <string>
#include <vector>


using namespace charm;


using stringy_list = std::vector <std::string>;


struct Tamparams
{ bool allow_anisotropic_pixelviews = false;

//  f64 workband_mid = 2000.0;
  f64 workband_mid = 1700.0;
  f64 workband_hei = 2000.0;
  f64 workband_elevstop = 0.0;

  f64 escaband_mid = 4875.0;
//  f64 escaband_hei = 3750.0;
  f64 escaband_hei = 4350.0;
  f64 escband_elevstop = 4875;

  f64 collband_mid = 7950.0;
  f64 collband_hei = 2400.0;
  f64 collabband_elevstop = 7950.0;

  f64 table_scale_factor = 0.5;

  f64 pb_snapback_interp_time = 0.4;
  f64 lateral_slide_interp_time = 0.3;
  f64 pb_snapback_fade_time = 0.5;

  f64 sono_choosist_furl_time = 0.35;

  f64 room_minify_factor = 0.478128;  // that's 4372mm / 9144mm, folks...

  f64 disposal_speed_threshold = 400.0;  // mm per sec, is the thing.

  static Tamparams ur_params;
  static std::vector <Tamparams *> tampa_stack;
  static Tamparams *Current ();
};


namespace charm
{ class AudioMessenger; }


struct Tamglobals
{ f64 frame_tm = 0.0;
  f64 frame_dt = 0.0;

  Node *front_wall = NULL;
  Node *left_wall = NULL;
  Node *wallpaper = NULL;
  Node *tablecloth = NULL;
  Node *conveyor = NULL;
  Node *windshield = NULL;

  std::vector <Node *> construction_marks;
  InterpColor construction_marks_color;

  InterpColor escatom_bbox_color;
  InterpColor tabatom_bbox_color;

  InterpVect room_scaler;
  bool room_is_scaled_oto = true;
  f64 cur_elev_stop = 0.0;

  AudioMessenger *sono_hermes = NULL;

  static Tamglobals ur_globals;
  static Tamglobals *Only ();
};


#endif
