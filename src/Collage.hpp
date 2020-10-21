#ifndef MIXES_MAKES
#define MIXES_MAKES

#include <Node.hpp>
#include <Renderable.hpp>

#include <ch_ptr.hpp>

#include <bgfx_utils.hpp>
#include <charm_glm.hpp>


namespace charm
{

class ClipInfo;
class FilmInfo;
class VideoTexture;

class CollageBackground : public Renderable
{
 public:
  CollageBackground (u16 _stencil_val);
  ~CollageBackground () override;

  void SetColor (glm::vec4 const &_color);
  glm::vec4 const &GetColor () const;

  void Draw (u16 _view_id) override;

 protected:
  glm::vec4 m_color;
  u16 m_stencil_val;

  bgfx::ProgramHandle m_program;
  bgfx::UniformHandle m_uni_over;
  bgfx::UniformHandle m_uni_up;
  bgfx::UniformHandle m_uni_color;
};

class CollageMatte : public Renderable
{
 public:
  CollageMatte (FilmInfo const &_film, ClipInfo const &_clip,
                u16 _stencil_val);

  ~CollageMatte () override;

  void Draw (u16 _view_id) override;

 protected:
  ch_ptr<VideoTexture> m_video_texture;

  u16 m_stencil_val;
};

class Collage : public Node
{
 public:
  Collage (u32 _count, std::vector<FilmInfo> const &_films,
           f64 _width, f64 _height);
};

class CollageBand : public Node
{
 public:
  CollageBand (f64 _band_width, f64 _band_height,
               std::vector<FilmInfo> const &_films);
};

}


#endif
