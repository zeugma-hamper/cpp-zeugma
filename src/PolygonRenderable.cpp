
//
// (c) treadle & loam, provisioners llc
//

#include "PolygonRenderable.h"

#include "Node.hpp"


namespace zeugma  {


void *PolygonRenderable::tessy_obj = NULL;


static const char *color_unif_s = "u_color";


PolygonRenderable::PolygonRenderable ()  :  Renderable (),
                                            should_fill (true),
                                            should_edge (false),
                                            should_close (true),
                                            shad_prog {BGFX_INVALID_HANDLE},
                                            fill_vbuf {BGFX_INVALID_HANDLE},
                                            edge_vbuf {BGFX_INVALID_HANDLE}
{ if (! tessy_obj)
    tessy_obj = gluNewTess ();

  unif_primc = bgfx::createUniform (color_unif_s, bgfx::UniformType::Vec4);

  bx::FilePath shader_path = "primal_vs.bin";
  bgfx::ShaderHandle vs = CreateShader (shader_path);
  shader_path = bx::StringView("primal_fs.bin");
  bgfx::ShaderHandle fs = CreateShader (shader_path);

  bgfx::VertexLayout layabout;
  layabout.begin ()
    . add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    . end ();

  fill_vbuf
    = bgfx::createDynamicVertexBuffer (1001, layabout, BGFX_BUFFER_ALLOW_RESIZE);
  edge_vbuf
    = bgfx::createDynamicVertexBuffer (1001, layabout, BGFX_BUFFER_ALLOW_RESIZE);

  if (bgfx::isValid (vs)  &&  bgfx::isValid (fs))
    shad_prog = bgfx::createProgram(vs, fs, true);
  else
    { bgfx::destroy (vs);
      bgfx::destroy (fs);
    }
}

PolygonRenderable::~PolygonRenderable ()
{ bgfx::destroy (fill_vbuf);
  bgfx::destroy (edge_vbuf);
  bgfx::destroy (shad_prog);
  bgfx::destroy (unif_primc);
}


static PolygonRenderable *ephem_pr = NULL;
static i64 trifan_prgrs = -1;
static glm::vec3 trifan_vrts[2];
static i64 strippy_prog = -1;
static glm::vec3 strippy_vs[2];


static void PRBeginCB (GLenum mode, void *bonus)
{ if (! bonus)
    return;
  ephem_pr = (PolygonRenderable *)bonus;
  if (mode == GL_TRIANGLE_FAN)
    { trifan_prgrs = 0;
      mode = GL_TRIANGLES;
    }
  else if (mode == GL_TRIANGLE_STRIP)
    { strippy_prog = 0;
      mode = GL_TRIANGLES;
    }
  ephem_pr->gl_mode = mode;
}

static void PRVertexCB (void *pvas, void */*bonus*/)
{ v3f64 *vrt = (v3f64 *)pvas;
  glm::vec3 vv (as_glm (*vrt));
  if (trifan_prgrs < 0  &&  strippy_prog < 0)
    { if (ephem_pr)
        ephem_pr->tessd_verts . push_back (vv);
    }
  else if (trifan_prgrs  >=  0)
    { if (trifan_prgrs < 2)
        trifan_vrts[trifan_prgrs] = vv;
      else
        { ephem_pr->tessd_verts . push_back (trifan_vrts[0]);
          ephem_pr->tessd_verts . push_back (trifan_vrts[1]);
          ephem_pr->tessd_verts . push_back (vv);
          trifan_vrts[1] = vv;
        }
      trifan_prgrs++;
    }
  else
    { if (strippy_prog < 2)
        strippy_vs[strippy_prog] = vv;
      else
        { ephem_pr->tessd_verts . push_back (strippy_vs[0]);
          ephem_pr->tessd_verts . push_back (strippy_vs[1]);
          ephem_pr->tessd_verts . push_back (vv);
          strippy_vs[0] = strippy_vs[1];
          strippy_vs[1] = vv;
        }
      strippy_prog++;
    }
}

static void PREndCB (void */*bonus*/)
{ if (trifan_prgrs >= 0)
    trifan_prgrs = -1;
  else if (strippy_prog >= 0)
    strippy_prog = -1;
  ephem_pr = NULL;
}

static void PRErrorCB (GLenum err, void */*bonus*/)
{ fprintf (stderr, "Holy gol-durn cripes! error in PRErrorCB: <%d>\n", err);
  fprintf (stderr, "... which is this: [%s]\n", gluErrorString (err));
}


void PolygonRenderable::PopulateRawVerts ()
{ raw_verts . clear ();

  if (should_fill)
    { raw_verts_f64 . clear ();

      for (auto &vzft  :  verts)
        { raw_verts_f64 . push_back (vzft.val);
          raw_verts . push_back (as_glm (vzft.val));
        }
      if (raw_verts . size () > 2)
        { raw_verts_f64 . push_back (raw_verts_f64[0]);
          raw_verts . push_back (raw_verts[0]);
        }
    }
  else
    { for (auto &vzft  :  verts)
        raw_verts . push_back (as_glm (vzft.val));
      if (should_close  &&  raw_verts . size () > 2)
        raw_verts . push_back (raw_verts[0]);
    }
}


void PolygonRenderable::SpankularlyTesselate ()
{ // do we need to check if this next is necessary? or does 'spanking_time'
  // capture all the circumstances we'd ever need?
  PopulateRawVerts ();

  if (should_fill)
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

      tessd_verts . clear ();
      gluTessBeginPolygon (gt, this);
      gluTessBeginContour (gt);
      for (auto &vrt  :  raw_verts_f64)
        { // hm...
          gluTessVertex (gt, &vrt.x,  &vrt.x);
        }
      gluTessEndContour (gt);
      gluTessEndPolygon (gt);

      ts_vrt_cnt = tessd_verts . size ();

      if (! bgfx::isValid (fill_vbuf))
        return;

      const bgfx::Memory *memmy
        = bgfx::copy (tessd_verts . data (),
                      ts_vrt_cnt * sizeof (glm::vec3));
      bgfx::update (fill_vbuf, 0, memmy);
    }

  if (should_edge)
    { const bgfx::Memory *mimmy
        = bgfx::copy (raw_verts . data (),
                      raw_verts . size () * sizeof (glm::vec3));
      bgfx::update (edge_vbuf, 0, mimmy);
    }

  spanking_time = false;
}


void PolygonRenderable::Draw (u16 vyu_id)
{ if (spanking_time)
    SpankularlyTesselate ();
spanking_time = true;

  if (should_fill)
    { bgfx::setTransform (&(m_node -> GetAbsoluteTransformation ().model));
      bgfx::setVertexBuffer (0, fill_vbuf, 0, ts_vrt_cnt);
      u64 prim
        = (gl_mode == GL_TRIANGLE_STRIP  ?  BGFX_STATE_PT_TRISTRIP  :  0x0);
      bgfx::setState (BGFX_STATE_WRITE_RGB
                      |  prim
                      |  BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                                BGFX_STATE_BLEND_INV_SRC_ALPHA)
                      |  BGFX_STATE_WRITE_Z);

      ZeColor use_col = m_cumu_adjc * fill_iro.val;
      bgfx::setUniform (unif_primc, glm::value_ptr (as_glm (use_col)));

      bgfx::submit (vyu_id, shad_prog, m_graph_id);
    }

  if (should_edge)
    { bgfx::setTransform (&(m_node -> GetAbsoluteTransformation ().model));
      bgfx::setVertexBuffer (0, edge_vbuf, 0, raw_verts . size ());
      bgfx::setState (BGFX_STATE_WRITE_RGB
                      |  BGFX_STATE_PT_LINESTRIP
                      |  BGFX_STATE_LINEAA
                      |  BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                                BGFX_STATE_BLEND_INV_SRC_ALPHA)
                      |  BGFX_STATE_WRITE_Z);

      ZeColor use_col = m_cumu_adjc * edge_iro.val;
      bgfx::setUniform (unif_primc, glm::value_ptr (as_glm (use_col)));

      bgfx::submit (vyu_id, shad_prog, m_graph_id);
    }
}


}  // namespace zeugma sails into the west with the elves
