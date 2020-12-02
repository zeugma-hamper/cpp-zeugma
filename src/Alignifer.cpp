
#include "Alignifer.h"

#include "CoGrappler.h"
#include "ScGrappler.h"
#include "TrGrappler.h"


using namespace charm;


void Alignifer::WellAndTrulyConstruct ()
{ loc . MakeBecomeLikable ();
  sca . MakeBecomeLikable ();

  CoGrappler *cog = new CoGrappler;
  cog -> SetName ("alignment");
  AppendGrappler (cog);

  ScGrappler *scg = new ScGrappler (sca);
  scg -> SetName ("scale");
  AppendGrappler (scg);

  TrGrappler *trg = new TrGrappler (loc);
  trg -> SetName ("loc");
  AppendGrappler (trg);
}


Alignifer::Alignifer ()  :  Zeubject (), Node (), sca (Vect::onesv),
                            ovr (Vect::xaxis), upp (Vect::yaxis),
                            nrm (Vect::zaxis)
{ WellAndTrulyConstruct (); }


Alignifer::Alignifer (Renderable *ren)  :  Zeubject (), Node (ren),
                                           sca (Vect::onesv),
                                           ovr (Vect::xaxis), upp (Vect::yaxis),
                                           nrm (Vect::zaxis)
{ WellAndTrulyConstruct (); }



void Alignifer::AlignOverUp (const Vect &ov, const Vect &up)
{ Vect o = ov . Norm ();
  Vect u = up . Norm ();
  Vect n = o . Cross (u);

  if (CoGrappler *g = dynamic_cast <CoGrappler *> (FindGrappler ("alignment")))
    g -> SetViaNormalizedBasisVectors (o, u, n);

  ovr = o;  upp = u;  nrm = n;
}
