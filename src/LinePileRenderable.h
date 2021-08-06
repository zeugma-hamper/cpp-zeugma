
#ifndef LINE_PILE_RENDERABLE_STREWS_STRAW_ON_THE_BAARN_FLOOR
#define LINE_PILE_RENDERABLE_STREWS_STRAW_ON_THE_BAARN_FLOOR


#include "Renderable.hpp"

#include "GeomFumble.h"

#include "ZoftThing.h"

#include "bgfx_utils.hpp"

#include <vector>


namespace zeugma  {


class LinePileRenderable  :  public Renderable
{ public:
  std::vector <G::Segment> lines;
  ZoftColor lines_iro;
  bool needs_refreshment;

  bgfx::ProgramHandle shad_prog;
  bgfx::DynamicVertexBufferHandle vbuf;
  bgfx::UniformHandle unif_linesc;


  LinePileRenderable ();
  ~LinePileRenderable ()  override;

  void ClearAllLines ()
    { lines . clear ();  needs_refreshment = true; }

  i64 NumLines ()  const
    { return lines . size (); }
  const G::Segment &NthLine (i64 ind)  const
    { return lines . at (ind); }
  G::Segment &NthLineModifiable (i64 ind)
    { needs_refreshment = true;  return lines . at (ind); }

  void AppendLine (const G::Segment &l)
    { lines . push_back (l);  needs_refreshment = true; }
  void AppendLine (const Vect &p1, const Vect &p2)
    { lines . push_back ({p1, p2});  needs_refreshment = true; }

  const ZeColor &LinesColor ()  const
    { return lines_iro.val; }
  void SetLinesColor (const ZeColor &c)
    { lines_iro . Set (c); }
  void SetLinesColor (const ZoftColor &zc)
    { lines_iro . BecomeLike (zc); }
  ZoftColor &LinesColorZoft ()
    { return lines_iro; }

  void Draw (u16 vyu_id)  override;
};


}


#endif
