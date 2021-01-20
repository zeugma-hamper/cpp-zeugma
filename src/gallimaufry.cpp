
#include "gallimaufry.h"

#include "LinePileRenderable.h"


using namespace charm;


Alignifer *PlusTimeWonderBundt (f64 wid, f64 hei, f64 spacing, f64 reach_frac)
{ i64 hrz_cnt = (i64) floor (0.5 * wid / spacing);
  i64 vrt_cnt = (i64) floor (0.5 * hei / spacing);

  if (hrz_cnt > 0
      &&  hrz_cnt * spacing + reach_frac * 0.5 * spacing  >  0.5 * wid)
    hrz_cnt--;

  if (vrt_cnt > 0
      &&  vrt_cnt * spacing + reach_frac * 0.5 * spacing  >  0.5 * hei)
    vrt_cnt--;

  LinePileRenderable *lipre = new LinePileRenderable;

  Vect up = 0.5 * reach_frac * spacing * Vect::yaxis;
  Vect ov = 0.5 * reach_frac * spacing * Vect::xaxis;
  for (i64 y = -vrt_cnt  ;  y <= vrt_cnt  ;  ++y)
    for (i64 x = -hrz_cnt  ;  x <= hrz_cnt  ;  ++x)
      { Vect cent = x * spacing * Vect::xaxis  +  y * spacing * Vect::yaxis;
        lipre -> AppendLine (cent - ov, cent + ov);
        lipre -> AppendLine (cent - up, cent + up);
      }

  Alignifer *plusno = new Alignifer (lipre);
  return plusno;
}
