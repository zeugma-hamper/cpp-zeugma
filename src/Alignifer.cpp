
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
  scg -> SetName ("loc");
  AppendGrappler (trg);
}


Alignifer::Alignifer ()  :  Zeubject (), Node (), sca (Vect::onesv)
{ WellAndTrulyConstruct (); }


Alignifer::Alignifer (Renderable *ren)  :  Zeubject (), Node (ren),
                                           sca (Vect::onesv)
{ WellAndTrulyConstruct (); }


void Alignifer::AlignOverUp (const Vect &ov, const Vect &up)
{ Vect o = ov . Norm ();
  Vect u = up . Norm ();
  Vect n = o . Cross (u);

  if (CoGrappler *g = dynamic_cast <CoGrappler *> (FindGrappler ("alignment")))
    { g -> PointMatrix () . Load (o.x, o.y, o.z, 0.0,
                                  u.x, u.y, u.z, 0.0,
                                  n.x, n.y, n.z, 0.0,
                                  0.0, 0.0, 0.0, 1.0);
      g -> InversePointMatrix () . Load (o.x, u.x, n.x, 0.0,
                                         o.y, u.y, n.y, 0.0,
                                         o.z, u.z, n.z, 0.0,
                                         0.0, 0.0, 0.0, 1.0);
      g -> SetNormalMatrix (g -> PointMatrix ());
      g -> SetInverseNormalMatrix (g -> InversePointMatrix ());
    }
}
