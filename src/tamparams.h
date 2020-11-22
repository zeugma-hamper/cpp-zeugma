
#ifndef TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM
#define TAMPARAMS_ETCHED_ON_STONE_TABLETS_OR_AT_LEAST_STYROFOAM


#include "base_types.hpp"

#include "Node.hpp"

#include <vector>


using namespace charm;


struct Tamparams
{ bool allow_anisotropic_pixelviews = false;

  f64 table_scale_factor = 0.5;

  Node *wallpaper = NULL;
  Node *tablecloth = NULL;
  Node *conveyor = NULL;
  Node *windshield = NULL;

  static Tamparams ur_params;
  static std::vector <Tamparams *> tampa_stack;
  static Tamparams *Current ();
};


#endif
