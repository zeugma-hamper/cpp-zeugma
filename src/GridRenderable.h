
//
// (c) treadle & loam, provisioners llc
//

#ifndef GRID_RENDERABLE_MAKES_DESCARTES_HAPPY_BUT_WHAT_ABOUT_MISTER_POLAR
#define GRID_RENDERABLE_MAKES_DESCARTES_HAPPY_BUT_WHAT_ABOUT_MISTER_POLAR


#include <Renderable.hpp>

#include "ZoftThing.h"

#include "bgfx_utils.hpp"


namespace zeugma  {


class GridRenderable  :  public Renderable
{ public:
  bgfx::ProgramHandle prog;
  ZoftVect cnt;
  ZoftFloat wid, hei;
  ZoftVect warp, weft;
  ZoftColor grid_iro;
  ZoftFloat disc_rad;
  ZoftVect disc_cnt;
  ZoftFloat span_frc;

  bgfx::ProgramHandle shad_prog;
  bgfx::DynamicVertexBufferHandle vbuf;
  bgfx::UniformHandle unif_gridc;
  bgfx::UniformHandle unif_warp;
  bgfx::UniformHandle unif_weft;
  bgfx::UniformHandle unif_dsc_cntrad;
  bgfx::UniformHandle unif_span_frc;
  bgfx::UniformHandle unif_fullmod_mat;

  GridRenderable ();
  ~GridRenderable ();

  const Vect &Center ()  const
    { return cnt.val; }
  void SetCenter (const Vect &c)
    { cnt . Set (c); }
  ZoftVect &CenterZoft ()
    { return cnt; }

  const f64 &Width ()  const
    { return wid.val; }
  void SetWidth (f64 w)
    { wid . Set (w); }
  ZoftFloat &WidthZoft ()
    { return wid; }

  const f64 &Height ()  const
    { return hei.val; }
  void SetHeight (f64 h)
    { hei . Set (h); }
  ZoftFloat &HeightZoft ()
    { return hei; }

  const Vect &Warp ()  const
    { return warp.val; }
  void SetWarp (const Vect &wa)
    { warp . Set (wa); }
  ZoftVect WarpZoft ()
    { return warp; }

  const Vect &Weft ()  const
    { return weft.val; }
  void SetWeft (const Vect &we)
    { weft . Set (we); }
  ZoftVect WeftZoft ()
    { return weft; }

  const ZeColor &GridColor ()  const
    { return grid_iro.val; }
  void SetGridColor (const ZeColor &gc)
    { grid_iro . Set (gc); }
  ZoftColor &GridColorZoft ()
    { return grid_iro; }

  f64 DiscRadius ()  const
    { return disc_rad; }
  void SetDiscRadius (f64 dr)
    { disc_rad = dr; }
  ZoftFloat &DiscRadiusZoft ()
    { return disc_rad; }

  const Vect &DiscCenter ()  const
    { return disc_cnt; }
  void SetDiscCenter (const Vect &dc)
    { disc_cnt . Set (dc); }
  ZoftVect &DiscCenterZoft ()
    { return disc_cnt; }

  f64 SpanFraction ()  const
    { return span_frc.val; }
  void SetSpanFraction (f64 sf)
    { span_frc . Set (sf); }
  ZoftFloat &SpanFractionZoft ()
    { return span_frc; }

  void Draw (u16 vuy_id)  override;
};


}


#endif
