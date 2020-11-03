
#include "PolygonRenderable.h"

#include "Node.hpp"

#include "vector_interop.hpp"


namespace charm  {


void *PolygonRenderable::tessy_obj = NULL;


static const char *color_unif_s = "u_color";


PolygonRenderable::PolygonRenderable ()  :  Renderable (),
                                            shad_prog {BGFX_INVALID_HANDLE},
                                            vert_buf {BGFX_INVALID_HANDLE},
                                            farbe { 1.0, 1.0, 0.0, 0.3 }
{ if (! tessy_obj)
    tessy_obj = gluNewTess ();

  unif_farbe = bgfx::createUniform (color_unif_s, bgfx::UniformType::Vec4);

  bx::FilePath shader_path = "primal_vs.bin";
  bgfx::ShaderHandle vs = CreateShader (shader_path);
  shader_path = bx::StringView("primal_fs.bin");
  bgfx::ShaderHandle fs = CreateShader (shader_path);

  bgfx::VertexLayout layabout;
  layabout.begin ()
    . add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    . end ();

  vert_buf
    = bgfx::createDynamicVertexBuffer (1001, layabout, BGFX_BUFFER_ALLOW_RESIZE);

  if (bgfx::isValid (vs)  &&  bgfx::isValid (fs))
    shad_prog = bgfx::createProgram(vs, fs, true);
  else
    { bgfx::destroy (vs);
      bgfx::destroy (fs);
    }
}

PolygonRenderable::~PolygonRenderable ()
{ bgfx::destroy (vert_buf);
  bgfx::destroy (shad_prog);
  bgfx::destroy (unif_farbe);
}


static PolygonRenderable *ephem_pr = NULL;
static i64 trifan_prgrs = -1;
static glm::vec3 trifan_vrts[2];


static void PRBeginCB (GLenum mode, void *bonus)
{ if (! bonus)
    return;
  ephem_pr = (PolygonRenderable *)bonus;
  if (mode == GL_TRIANGLE_FAN)
    { trifan_prgrs = 0;
      mode = GL_TRIANGLES;
    }
  ephem_pr->gl_mode = mode;
}

static void PRVertexCB (void *pvas, void *bonus)
{ v3f64 *vrt = (v3f64 *)pvas;
  glm::vec3 vv (as_glm (*vrt));
  if (trifan_prgrs < 0)
    { if (ephem_pr)
        ephem_pr->ts_verts . push_back (vv);
    }
  else
    { if (trifan_prgrs < 2)
        trifan_vrts[trifan_prgrs] = vv;
      else
        { ephem_pr->ts_verts . push_back (trifan_vrts[0]);
          ephem_pr->ts_verts . push_back (trifan_vrts[1]);
          ephem_pr->ts_verts . push_back (vv);
          trifan_vrts[1] = vv;
        }
      trifan_prgrs++;
    }
}

static void PREndCB (void *bonus)
{ if (trifan_prgrs >= 0)
    trifan_prgrs = -1;
  ephem_pr = NULL;
}

static void PRErrorCB (GLenum err, void *bonus)
{ fprintf (stderr, "Holy gol-durn cripes! error in PRErrorCB: <%d>\n", err);
  fprintf (stderr, "... which is this: [%s]\n", gluErrorString (err));
}


void PolygonRenderable::SpankularlyTesselate ()
{ GLUtesselator *gt = (GLUtesselator *)tessy_obj;

  gluTessProperty (gt, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
//  gluTessProperty (gt, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

  gluTessCallback (gt, GLU_TESS_BEGIN_DATA,
                   reinterpret_cast <GLUCB> (&PRBeginCB));
  gluTessCallback (gt, GLU_TESS_VERTEX_DATA,
                   reinterpret_cast <GLUCB> (&PRVertexCB));
  gluTessCallback (gt, GLU_TESS_END_DATA,
                   reinterpret_cast <GLUCB> (&PREndCB));
  gluTessCallback (gt, GLU_TESS_ERROR_DATA,
                   reinterpret_cast <GLUCB> (&PRErrorCB));

  ts_verts . clear ();
  gluTessBeginPolygon (gt, this);
  gluTessBeginContour (gt);
  for (auto &it  :  verts)
    gluTessVertex (gt, &it.val.x,  &it.val.x);
  gluTessEndContour (gt);
  gluTessEndPolygon (gt);

  ts_vrt_cnt = ts_verts . size ();

  if (! bgfx::isValid (vert_buf))
    return;

  const bgfx::Memory *memmy = bgfx::copy (ts_verts . data (),
                                    ts_vrt_cnt * sizeof (glm::vec3));
  bgfx::update (vert_buf, 0, memmy);
}


void PolygonRenderable::Draw (u16 vyu_id)
{ bgfx::setTransform (&(m_node -> GetAbsoluteTransformation ().model));
  bgfx::setVertexBuffer (0, vert_buf, 0, ts_vrt_cnt);
  u64 prim = (gl_mode == GL_TRIANGLE_STRIP  ?  BGFX_STATE_PT_TRISTRIP  :  0x0);
  bgfx::setState (BGFX_STATE_WRITE_RGB
                  |  prim
                  |  BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                            BGFX_STATE_BLEND_INV_SRC_ALPHA)
                  |  BGFX_STATE_WRITE_Z);

  bgfx::setUniform(unif_farbe, glm::value_ptr (farbe));

  bgfx::submit (vyu_id, shad_prog, m_graph_id);
}


}  // namespace charm sails into the west with the elves
