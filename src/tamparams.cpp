
#include "tamparams.h"


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


//
/// globals, don't you know
//

Tamglobals Tamglobals::ur_globals;

Tamglobals *Tamglobals::Only ()
{ return &ur_globals; }
