
//
// (c) treadle & loam, provisioners llc
//

#ifndef A_BIT_OF_FLAVOR
#define A_BIT_OF_FLAVOR


#include <Renderable.hpp>

#include <bgfx_utils.hpp>


namespace zeugma  {


class TexturedRenderable final : public Renderable
{
 public:

  TexturedRenderable ();
  TexturedRenderable (TextureParticulars const &_tp);

  void SetTexture (TextureParticulars const &_tp);
  TextureParticulars const &GetTexture () const;

  void Draw (u16 _view_id) override;

 protected:

  void AdaptGeometryToTexture ();

  bgfx::ProgramHandle m_program;
  bgfx::VertexBufferHandle m_vbuffer;
  bgfx::UniformHandle m_uni_over;
  bgfx::UniformHandle m_uni_up;
  bgfx::UniformHandle m_uni_wh;
  bgfx::UniformHandle m_uni_texture;
  bgfx::UniformHandle m_uni_adj_iro;
  TextureParticulars m_particulars;

};


}


#endif  // A_BIT_OF_FLAVOR
