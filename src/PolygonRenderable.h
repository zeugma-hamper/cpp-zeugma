
#ifndef POLYGON_RENDERABLE_LIVES_NEXT_DOOR_TO_MISTER_A_SQUARE
#define POLYGON_RENDERABLE_LIVES_NEXT_DOOR_TO_MISTER_A_SQUARE


#include "Renderable.hpp"

#include "ZoftThing.h"

#include "vector_interop.hpp"

#include "bgfx_utils.hpp"

#include <vector>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


namespace charm  {


class PolygonRenderable  :  public Renderable
{ public:
  std::vector <ZoftVect> verts;
  ZoftColor fill_iro, edge_iro;
  bool should_fill, should_edge;
  bool should_close;
  bool spanking_time;
  static void *tessy_obj;
  std::vector <glm::vec3> raw_verts;
  std::vector <glm::vec3> tessd_verts;
  std::vector <Vect> raw_verts_f64;
  i64 ts_vrt_cnt;
  bgfx::ProgramHandle shad_prog;
  bgfx::DynamicVertexBufferHandle fill_vbuf;
  bgfx::DynamicVertexBufferHandle edge_vbuf;
  bgfx::UniformHandle unif_primc;
  GLenum gl_mode;

  typedef void (*GLUCB) ();


  PolygonRenderable ();
  ~PolygonRenderable ()  override;

  void AppendVertex (const Vect &v)
    { verts . push_back (ZoftVect (v));  spanking_time = true; }

  void AppendVertex (const ZoftVect &z)
    { verts . push_back (z);  spanking_time = true; }

  void ClearVertices ()
    { verts . clear ();  spanking_time = true; }

  i64 VertexCount ()  const
    { return verts . size (); }  // may you roast in hell, stl


  void AppendRawVertex (const Vect &v)
    { raw_verts . push_back (as_glm (v));  spanking_time = true; }

  void AppendVertex (const glm::vec3 &v)
    { raw_verts . push_back (v);  spanking_time = true; }

  void ClearRawVertices ()
    { raw_verts . clear ();  spanking_time = true; }

  i64 RawVertexCount ()  const
    { return raw_verts . size (); }  // may you broil in hades, stl


  bool ShouldFill ()  const
    { return should_fill; }
  void SetShouldFill (bool sf)
    { should_fill = sf;  }

  bool ShouldEdge ()  const
    { return should_edge; }
  void SetShouldEdge (bool ss)
    { should_edge = ss;  }

  bool ShouldClose ()  const
    { return should_close; }
  void SetShouldClose (bool sc)
    { should_close = sc;  }

  const ZeColor &FillColor ()
    { return fill_iro.val; }
  ZoftColor &FillColorZoft ()
    { return fill_iro; }
  void SetFillColor (const ZeColor &c)
    { fill_iro . Set (c); }
  void SetFillColor (const ZoftColor &zc)
    { fill_iro . BecomeLike (zc); }

  const ZeColor &EdgeColor ()
    { return edge_iro.val; }
  ZoftColor &EdgeColorZoft ()
    { return edge_iro; }
  void SetEdgeColor (const ZeColor &c)
    { edge_iro . Set (c); }
  void SetEdgeColor (const ZoftColor &zc)
    { edge_iro . BecomeLike (zc); }

  virtual void PopulateRawVerts ();
  void SpankularlyTesselate ();
  void Draw (u16 vyu_id)  override;
};


}  // bust out the hankies for namespace charm


#endif
