
#ifndef TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM
#define TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM


#include "base_types.hpp"

#include "Node.hpp"
#include "InterpZoft.h"

#include "OeuvreAfterlife.h"

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
  f64 escband_elevstop = 1950.0;

  f64 collband_mid = 7950.0;
  f64 collband_hei = 2400.0;
  f64 collabband_elevstop = 5500.0;

  f64 coll_scale = 2266.6;
  f64 coll_spacing = 2600.0;
  f64 coll_slosh_time = 4.75;

  f64 table_escatom_scale_factor = 0.5;
  f64 table_flmatom_scale_factor = 0.2;

  f64 table_grid_opacity = 0.075;

  f64 pb_snapback_interp_time = 0.4;
  f64 lateral_slide_interp_time = 0.3;
  f64 pb_snapback_fade_time = 0.5;

  f64 sono_choosist_furl_time = 0.35;

  f64 room_minify_factor = 0.478128;  // that's 4372mm / 9144mm, folks...

  f64 disposal_speed_threshold = 900.0;  // mm per sec, is the thing.


  i64 clapper_vis_frame_cnt = 2;  // note that one frame is sometimes too fast

  bool ascension_auto_follow = true;

  f64 asc_table_slide_time = 4.04;
  f64 asc_first_rise_time = 5.05;
  f64 asc_enbloaten_time = 2.0;
  f64 asc_before_preso_hold_time = 0.5;
  f64 asc_presentation_time = 3.0;
  f64 asc_after_preso_hold_time = 0.5;
  f64 asc_ensvelten_time = 1.5;
  f64 asc_second_rise_time = 6.06;
  f64 asc_enter_heaven_time = 0.0;

  std::string asc_table_slide_audio = "";
  std::string asc_first_rise_audio = "";
  std::string asc_enbloaten_audio = "";
  std::string asc_before_preso_hold_audio = "";
  std::string asc_presentation_audio = "";
  std::string asc_after_preso_hold_audio = "";
  std::string asc_ensvelten_audio = "";
  std::string asc_second_rise_audio = "";
  std::string asc_enter_heaven_audio = "";

// additional ascension-related timings

  f64 asc_collage_background_appear_time = 0.1;
  f64 asc_collage_background_grey_val = 0.15;


  static Tamparams ur_params;
  static std::vector <Tamparams *> tampa_stack;

  static Tamparams *Current ();
  static void PushCopy ();
  static void Pop ();

  static bool UpdateViaTOMLFile (const std::string &fname);
};


namespace charm
{ class AudioMessenger; }

class AtomicFreezone;

class Tampo;


struct Tamglobals
{ f64 frame_tm = 0.0;
  f64 frame_dt = 0.0;

  Node *front_wall = NULL;
  Node *left_wall = NULL;
  Node *wallpaper = NULL;
  Node *tablecloth = NULL;
  Node *conveyor = NULL;
  Node *fiducials = NULL;
  Node *windshield = NULL;

  Node *clapper_visuals = NULL;
  i64 clapper_cnt = -1;

  Node *wall_grids = NULL;
  InterpColor wall_grid_fader;
  bool wall_grid_active = false;

  AtomicFreezone *sterngerlach = NULL;
  OeuvreAfterlife *valhalla = NULL;

  Tampo *solo_tamp = NULL;

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
