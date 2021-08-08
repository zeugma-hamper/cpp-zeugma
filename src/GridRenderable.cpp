
//
// (c) treadle & loam, provisioners llc
//

#include "GridRenderable.h"

#include "Node.hpp"

#include "vector_interop.hpp"

#include <vector>


namespace zeugma  {


static const char *color_unif_s = "u_color";
static const char *warp_unif_s = "u_warp";
static const char *weft_unif_s = "u_weft";
static const char *cntrad_unif_s = "u_cntrad";
static const char *span_frac_unif_s = "u_span_frac";
static const char *full_model_mat_s = "u_full_model_mat";


GridRenderable::GridRenderable ()  :  Renderable (), wid (10.0), hei (10.0),
                                      warp (Vect::xaxis), weft (Vect::yaxis),
                                      shad_prog {BGFX_INVALID_HANDLE},
                                      vbuf {BGFX_INVALID_HANDLE},
                                      disc_rad (-1.0), span_frc (1.0)
{ unif_gridc = bgfx::createUniform (color_unif_s, bgfx::UniformType::Vec4);
  unif_warp = bgfx::createUniform (warp_unif_s, bgfx::UniformType::Vec4);
  unif_weft = bgfx::createUniform (weft_unif_s, bgfx::UniformType::Vec4);
  unif_dsc_cntrad = bgfx::createUniform (cntrad_unif_s, bgfx::UniformType::Vec4);
  unif_span_frc
    = bgfx::createUniform (span_frac_unif_s, bgfx::UniformType::Vec4);
  unif_fullmod_mat
    = bgfx::createUniform (full_model_mat_s, bgfx::UniformType::Mat4);

  bx::FilePath shader_path = "grid_vs.bin";
  bgfx::ShaderHandle vs = CreateShader (shader_path);
  shader_path = bx::StringView ("grid_fs.bin");
  bgfx::ShaderHandle fs = CreateShader (shader_path);

  if (bgfx::isValid (vs)  &&  bgfx::isValid (fs))
    shad_prog = bgfx::createProgram (vs, fs, true);
  else
    { bgfx::destroy (vs);
      bgfx::destroy (fs);
    }

  bgfx::VertexLayout lout;
  lout.begin ()
    . add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    . end ();

  vbuf = bgfx::createDynamicVertexBuffer (4, lout, BGFX_BUFFER_ALLOW_RESIZE);
 }


GridRenderable::~GridRenderable ()
{ bgfx::destroy (shad_prog);
  bgfx::destroy (vbuf);
}


static std::vector <glm::vec3> waystation;

void GridRenderable::Draw (u16 vyu_id)
{ waystation . clear ();

  Vect o = wid.val * m_over.val;
  Vect u = hei * m_up.val;
  Vect v = cnt.val - 0.5 * (o + u);
  waystation . push_back (as_glm (v));
  waystation . push_back (as_glm (v += u));
  waystation . push_back (as_glm (v += (o - u)));
  waystation . push_back (as_glm (v += u));

  const bgfx::Memory *m = bgfx::copy (waystation . data (),
                                      waystation . size () * sizeof (glm::vec3));
  bgfx::update (vbuf, 0, m);

  const glm::mat4 &abs_model_mat = m_node -> GetAbsoluteTransformation ().model;
  bgfx::setTransform (&abs_model_mat);
  bgfx::setVertexBuffer (0, vbuf, 0, 4);
  bgfx::setState (BGFX_STATE_WRITE_RGB
                  |  BGFX_STATE_WRITE_A
                  |  BGFX_STATE_PT_TRISTRIP
                  |  BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                            BGFX_STATE_BLEND_INV_SRC_ALPHA));
//                  |  BGFX_STATE_WRITE_Z);
  bgfx::setUniform (unif_gridc, glm::value_ptr (as_glm (grid_iro)));
  glm::vec4 wrp = {warp.val.x, warp.val.y, warp.val.z, 0.0};
  glm::vec4 wft = {weft.val.x, weft.val.y, weft.val.z, 0.0};
  bgfx::setUniform (unif_warp, glm::value_ptr (wrp));
  bgfx::setUniform (unif_weft, glm::value_ptr (wft));

  glm::vec4 dsc = {disc_cnt.val.x, disc_cnt.val.y, disc_cnt.val.z,
                   disc_rad.val};
  bgfx::setUniform (unif_dsc_cntrad, glm::value_ptr (dsc));

  glm::vec4 frc = {span_frc.val, 0.0, 0.0, 0.0};
  bgfx::setUniform (unif_span_frc, glm::value_ptr (frc));

  bgfx::setUniform (unif_fullmod_mat, glm::value_ptr (abs_model_mat));

  bgfx::submit (vyu_id, shad_prog, m_graph_id);
}


}
