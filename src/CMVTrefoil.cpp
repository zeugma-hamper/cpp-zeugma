#include <CMVTrefoil.h>

#include <Bolex.h>
#include <PlatonicMaes.h>

namespace charm
{

Bolex *CameraFromMaes (const PlatonicMaes &m)
{
  Bolex *cam = new Bolex;

  Vect nrm = m . Over () . Cross (m . Up ()) . Norm ();
  f64 dst = 0.8 * m . Width ();

  cam -> SetViewDist (dst);
  cam -> SetViewLoc (m . Loc ()  +  dst * nrm);
  cam -> SetViewCOI (m . Loc ());
  cam -> SetViewUp (m . Up ());

  cam -> SetProjectionType (Bolex::ProjType::PERSPECTIVE);

  cam -> SetViewHorizAngle (2.0 * atan (0.5 * m . Width () / dst));
  cam -> SetViewVertAngle (2.0 * atan (0.5 * m . Height () / dst));

  cam -> SetNearAndFarClipDist (0.1, 10.0 * dst);
  return cam;
}

i64 CMVTrefoil::Inhale (i64 ratch, f64 thyme)
{
  if (cam)  cam -> Inhale (ratch, thyme);
  if (maes)  maes -> Inhale (ratch, thyme);
  return 0;
}

}
