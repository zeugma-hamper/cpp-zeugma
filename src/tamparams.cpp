
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


//
/// globals, don't you know
//

Tamglobals Tamglobals::ur_globals;

Tamglobals *Tamglobals::Only ()
{ return &ur_globals; }
