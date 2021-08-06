
#include "LinePileRenderable.h"

#include "Node.hpp"

#include "vector_interop.hpp"


namespace zeugma  {


static const char *color_unif_s = "u_color";

LinePileRenderable::LinePileRenderable ()  :  Renderable (),
                                              needs_refreshment (true)
{ bx::FilePath shader_path = "primal_vs.bin";
  bgfx::ShaderHandle vs = CreateShader (shader_path);
  shader_path = bx::StringView("primal_fs.bin");
  bgfx::ShaderHandle fs = CreateShader (shader_path);

  bgfx::VertexLayout lout;
  lout.begin ()
    . add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
    . end ();

  vbuf = bgfx::createDynamicVertexBuffer (40, lout, BGFX_BUFFER_ALLOW_RESIZE);

  if (bgfx::isValid (vs)  &&  bgfx::isValid (fs))
    shad_prog = bgfx::createProgram(vs, fs, true);
  else
    { bgfx::destroy (vs);
      bgfx::destroy (fs);
    }

  unif_linesc = bgfx::createUniform (color_unif_s, bgfx::UniformType::Vec4);
}


LinePileRenderable::~LinePileRenderable ()
{ bgfx::destroy (vbuf);
  bgfx::destroy (shad_prog);
  bgfx::destroy (unif_linesc);
}


static std::vector <glm::vec3> glumlines;

void LinePileRenderable::Draw (u16 vyu_id)
{ if (needs_refreshment)
    { u32 cnt = 0;
      glumlines . clear ();
      for (const G::Segment &seg  :  lines)
        { glumlines . push_back (as_glm (seg.pt1));
          glumlines . push_back (as_glm (seg.pt2));
          cnt += 2;
        }
      const bgfx::Memory *memble
        = bgfx::copy (glumlines . data (), cnt * sizeof (glm::vec3));
      bgfx::update (vbuf, 0, memble);
      needs_refreshment = false;
    }

  bgfx::setTransform (&(m_node -> GetAbsoluteTransformation ().model));
  bgfx::setVertexBuffer (0, vbuf, 0, 2 * lines . size ());
  bgfx::setState (BGFX_STATE_WRITE_RGB
                  |  BGFX_STATE_PT_LINES
                  |  BGFX_STATE_LINEAA
                  |  BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                            BGFX_STATE_BLEND_INV_SRC_ALPHA));
//                  |  BGFX_STATE_WRITE_Z);

  ZeColor use_col = m_cumu_adjc * lines_iro.val;
  bgfx::setUniform (unif_linesc, glm::value_ptr (as_glm (use_col)));

  bgfx::submit (vyu_id, shad_prog, m_graph_id);
}


}
