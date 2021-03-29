
#include "tamparams.h"

#include "conjure-from-toml.h"

#include <regex>


using namespace charm;


//
/// params, you see
//

Tamparams Tamparams::ur_params;

std::vector <Tamparams *> Tamparams::tampa_stack;

Tamparams *Tamparams::Current ()
{ if (tampa_stack . empty ())
    return &ur_params;
  return tampa_stack . back ();
}


void Tamparams::PushCopy ()
{ Tamparams *cur_tp = Current ();
  Tamparams *novo_tp = new Tamparams;
  *novo_tp = *cur_tp;
  tampa_stack . push_back (novo_tp);
}


void Tamparams::Pop ()
{ if (tampa_stack . size ()  >  0)
    { Tamparams *moribund_tp = tampa_stack . back ();
      tampa_stack . pop_back ();
      delete moribund_tp;
    }
}


#define SLURPARAM(VAR,TYP)                                              \
  try                                                                   \
    { key = std::regex_replace (std::string (#VAR), std::regex ("_"), "-");  \
      tam->VAR = toml::get <TYP> (toml::find (tom, key));               \
    }                                                                   \
  catch (...)                                                           \
    { }

bool Tamparams::UpdateViaTOMLFile (const std::string &fname)
{ try
    { const toml::value &tom = LoadOrRecallTOMLByFilename (fname);
      Tamparams::PushCopy ();
      Tamparams *tam = Tamparams::Current ();
      std::string key;

      SLURPARAM (pushback_depth_scale, f64);
      SLURPARAM (pushback_slide_scale, f64);
      SLURPARAM (pushback_max_push, f64);
      SLURPARAM (pushback_max_pull, f64);

      SLURPARAM (workband_mid, f64);
      SLURPARAM (workband_hei, f64);
      SLURPARAM (workband_elevstop, f64);

      SLURPARAM (escaband_mid, f64);
      SLURPARAM (escaband_hei, f64);
      SLURPARAM (escband_elevstop, f64);

      SLURPARAM (escatom_wall_scale, f64);
      SLURPARAM (escatom_wall_count, i64);

      SLURPARAM (collband_mid, f64);
      SLURPARAM (collband_hei, f64);
      SLURPARAM (collabband_elevstop, f64);

      SLURPARAM (grauman_flick_wid, f64);
      SLURPARAM (grauman_flick_spacing, f64);

      SLURPARAM (coll_scale, f64);
      SLURPARAM (coll_spacing, f64);
      SLURPARAM (coll_slosh_time, f64);

      SLURPARAM (coll_leftmost_ordinal, i64);

      SLURPARAM (table_escatom_scale_factor, f64);
      SLURPARAM (table_flmatom_scale_factor, f64);

      SLURPARAM (table_grid_opacity, f64);

      SLURPARAM (pb_snapback_interp_time, f64);
      SLURPARAM (lateral_slide_interp_time, f64);
      SLURPARAM (pb_snapback_fade_time, f64);

      SLURPARAM (sono_choosist_furl_time, f64);

      SLURPARAM (room_minify_factor, f64);

      SLURPARAM (disposal_speed_threshold, f64);

      SLURPARAM (ascension_auto_follow, bool);

      SLURPARAM (asc_table_slide_time, f64);
      SLURPARAM (asc_first_rise_time, f64);
      SLURPARAM (asc_preso_vert_pos, f64);
      SLURPARAM (asc_enbloaten_time, f64);
      SLURPARAM (asc_before_preso_hold_time, f64);
      SLURPARAM (asc_presentation_time, f64);
      SLURPARAM (asc_after_preso_hold_time, f64);
      SLURPARAM (asc_ensvelten_time, f64);
      SLURPARAM (asc_second_rise_time, f64);
      SLURPARAM (asc_enter_heaven_time, f64);
      // un-uncommented the foregoing, since it don't hurt nuthin'...

      SLURPARAM (asc_table_slide_audio, std::string);
      SLURPARAM (asc_first_rise_audio, std::string);
      SLURPARAM (asc_enbloaten_audio, std::string);
      SLURPARAM (asc_before_preso_hold_audio, std::string);
      SLURPARAM (asc_presentation_audio, std::string);
      SLURPARAM (asc_after_preso_hold_audio, std::string);
      SLURPARAM (asc_ensvelten_audio, std::string);
      SLURPARAM (asc_second_rise_audio, std::string);
      SLURPARAM (asc_enter_heaven_audio, std::string);
      // ditto on the un-uncommentationizings

      SLURPARAM (asc_collage_background_appear_time, f64);
      SLURPARAM (asc_collage_background_grey_val, f64);
    }
  catch (...)
    { fprintf (stderr, "from Tamparams::UpdateViaTOMLFile: well, no.\n");
      return false;
    }

  return true;
}

#undef SLURPARAM



//
/// globals, don't you know
//

Tamglobals Tamglobals::ur_globals;

Tamglobals *Tamglobals::Only ()
{ return &ur_globals; }
