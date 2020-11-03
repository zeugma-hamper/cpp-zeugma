
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
//  std::vector <v3f64> packd_verts;
  bool spanking_time;
  static void *tessy_obj;
  std::vector <glm::vec3> ts_verts;
  i64 ts_vrt_cnt;
  glm::vec4 farbe;
  bgfx::ProgramHandle shad_prog;
  bgfx::DynamicVertexBufferHandle vert_buf;
  bgfx::UniformHandle unif_farbe;
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


  void SpankularlyTesselate ();
  void Draw (u16 vyu_id)  override;
};


}  // bust out the hankies for namespace charm


#endif
