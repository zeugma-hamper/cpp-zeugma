
#ifndef POLYGON_RENDERABLE_LIVES_NEXT_DOOR_TO_MISTER_A_SQUARE
#define POLYGON_RENDERABLE_LIVES_NEXT_DOOR_TO_MISTER_A_SQUARE


#include "Renderable.hpp"

#include "ZoftThing.h"

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
  bool should_fill, should_edge;
  glm::vec4 fill_iro, edge_iro;
//  std::vector <v3f64> packd_verts;
  bool spanking_time;
  static void *tessy_obj;
  std::vector <glm::vec3> raw_verts;
  std::vector <glm::vec3> tessd_verts;
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


  bool ShouldFill ()  const
    { return should_fill; }
  void SetShouldFill (bool sf)
    { should_fill = sf;  }
  bool ShouldEdge ()  const
    { return should_edge; }
  void SetShouldEdge (bool ss)
    { should_edge = ss;  }

  const glm::vec4 &FillColor ()
    { return fill_iro; }
  void SetFillColor (const glm::vec4 &c)
    { fill_iro = c; }
  const glm::vec4 &EdgeColor ()
    { return edge_iro; }
  void SetEdgeColor (const glm::vec4 &c)
    { edge_iro = c; }

  void SpankularlyTesselate ();
  void Draw (u16 vyu_id)  override;
};


}  // bust out the hankies for namespace charm


#endif
