
#include "tamparams.h"


Tamparams Tamparams::ur_params;

std::vector <Tamparams *> Tamparams::tampa_stack;


Tamparams *Tamparams::Current ()
{ if (tampa_stack . empty ())
    return &ur_params;
  return tampa_stack . back ();
}
