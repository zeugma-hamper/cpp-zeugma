#include <Collage.hpp>

#include <Matte.hpp>
#include <Node.hpp>
#include <VideoSystem.hpp>

#include <SumZoft.h>
#include <SinuZoft.h>

#include <DecodePipeline.hpp>

#include <vector_interop.hpp>

#include "global-params.h"

#include <random>

namespace charm
{

static const char *s_collage_bg_over = "u_over";
static const char *s_collage_bg_up = "u_up";
static const char *s_collage_bg_color = "u_color";

CollageBackground::CollageBackground (u16 _stencil_val)
  : Renderable (),
    m_color {0.0f, 0.0f, 0.0f, 1.0f},
    m_stencil_val {_stencil_val},
    m_program   {BGFX_INVALID_HANDLE},
    m_uni_over  {BGFX_INVALID_HANDLE},
    m_uni_up    {BGFX_INVALID_HANDLE},
    m_uni_color {BGFX_INVALID_HANDLE}
{
  m_uni_over  = bgfx::createUniform (s_collage_bg_over,  bgfx::UniformType::Vec4);
  m_uni_up    = bgfx::createUniform (s_collage_bg_up,    bgfx::UniformType::Vec4);
  m_uni_color = bgfx::createUniform (s_collage_bg_color, bgfx::UniformType::Vec4);

  ProgramResiduals ps = CreateProgram ("collage_bg_vs.bin", "collage_bg_fs.bin", true);
  m_program = ps.program;

  assert (bgfx::isValid(m_program));
}

CollageBackground::~CollageBackground ()
{
  bgfx::destroy (m_uni_color);
  bgfx::destroy (m_uni_up);
  bgfx::destroy (m_uni_over);
  bgfx::destroy (m_program);
}

void CollageBackground::SetColor (glm::vec4 const &_color)
{
  m_color = _color;
}

glm::vec4 const &CollageBackground::GetColor () const
{
  return m_color;
}

void CollageBackground::Draw (u16 _view_id)
{
  u64 const render_state = BGFX_STATE_WRITE_RGB |
                           BGFX_STATE_PT_TRISTRIP;

  u32 const stencil_state = BGFX_STENCIL_FUNC_REF(m_stencil_val) |
    BGFX_STENCIL_FUNC_RMASK(0xff) |
    BGFX_STENCIL_TEST_GREATER |
    BGFX_STENCIL_OP_FAIL_S_KEEP |
    BGFX_STENCIL_OP_FAIL_Z_KEEP | //not really needed
    BGFX_STENCIL_OP_PASS_Z_REPLACE;

  bgfx::setState(render_state);
  bgfx::setStencil(stencil_state);

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  bgfx::setUniform(m_uni_over, &over);
  bgfx::setUniform(m_uni_up, &up);

  bgfx::setUniform(m_uni_color, glm::value_ptr (m_color));

  bgfx::setVertexCount(4);
  bgfx::submit (_view_id, m_program);
}


CollageMatte::CollageMatte (FilmInfo const &_film, ClipInfo const &_clip,
                            u16 _stencil_val)
  : Renderable (),
    m_stencil_val(_stencil_val)
{
  v2i32 min = {i32 (_clip.geometry.dir_geometry.min[0]),i32 (_clip.geometry.dir_geometry.min[1])};
  v2i32 max = {i32 (_clip.geometry.dir_geometry.max[0]),i32 (_clip.geometry.dir_geometry.max[1])};

  VideoBrace brace = VideoSystem::GetSystem ()
    ->OpenMatte (std::string ("file://") + _film.film_path.string(),
                 _clip.start_time, _clip.start_time + _clip.duration,
                 _clip.frame_count, _clip.directory, min, max);
  m_video_texture = brace.video_texture;
}

CollageMatte::~CollageMatte ()
{
}

void CollageMatte::Draw (u16 _view_id)
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture(0)))
    return;

  u32 const stencil_state = BGFX_STENCIL_FUNC_REF(m_stencil_val) |
    BGFX_STENCIL_FUNC_RMASK(0xff) |
    (global_param_should_clip_collages
     ?  BGFX_STENCIL_TEST_EQUAL  :  BGFX_STENCIL_TEST_ALWAYS) |
    BGFX_STENCIL_OP_FAIL_S_KEEP |
    BGFX_STENCIL_OP_FAIL_Z_KEEP | //not really needed
    BGFX_STENCIL_OP_PASS_Z_KEEP;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);
  bgfx::setStencil(stencil_state);
  bgfx::setVertexCount(4);

  v2i32 const dim = m_video_texture->GetDimensions ();
  // v2i32 const matte_min = m_video_texture->GetMatteMin();
  // v2i32 const matte_max = m_video_texture->GetMatteMax();

  glm::vec4 vid_dim {dim.x, dim.y, 1.0f, 1.0f};
  glm::vec4 matte_dim {0.0f, 0.0f, dim.x, dim.y};

  bgfx::setUniform(m_video_texture->GetDimensionUniform(), glm::value_ptr (vid_dim));
  bgfx::setUniform(m_video_texture->GetMatteDimUniform(),  glm::value_ptr (matte_dim));

  glm::vec4 const over = glm::vec4 (as_glm (m_over), 0.0f);
  glm::vec4 const up = glm::vec4 (as_glm (m_up), 0.0f);
  bgfx::setUniform(m_video_texture->GetOverUniform(), &over);
  bgfx::setUniform(m_video_texture->GetUpUniform(), &up);

  bgfx::submit(_view_id, m_video_texture->GetProgram());
}

static u16 s_collage_stencil_base = 1;
Collage::Collage (u32 _count, std::vector<FilmInfo> const &_films,
                  f64 _width, f64 _height)
  : Node ()
{
  u16 const stencil_val = s_collage_stencil_base++;
  CollageBackground *bg = new CollageBackground (stencil_val);
  bg->SetOver (_width * Vect (1.0, 0.0, 0.0));
  bg->SetUp (_height * Vect (0.0, 1.0, 0.0));
  bg->SetColor (glm::vec4 (0.2f, 0.2f, 0.2f, 1.0f));
  Node *n = new Node;
  n->AppendRenderable(bg);
  AppendChild (n);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<>  film_distrib (0, _films.size ()-1);
  std::uniform_real_distribution<> width_distrib (-0.3 * _width, 0.3 * _width);
  std::uniform_real_distribution<> height_distrib (-0.3 * _height, 0.3 * _height);

  for (u32 i = 0; i < _count; ++i)
    {
      FilmInfo const &fm = _films[film_distrib (gen)];
      fprintf (stderr, "fm is %s\n", fm.abbreviation.c_str ());
      assert (fm.clips.size () > 1);
      std::uniform_int_distribution<> clip_distrib (0, fm.clips.size ()-1);
      ClipInfo const &cm = fm.clips[clip_distrib (gen)];

      CollageMatte *collage_matte = new CollageMatte (fm, cm, stencil_val);
      Node *matte_node = new Node;
      matte_node->AppendRenderable(collage_matte);
      matte_node->Scale(Vect (_width, _width, _width));
      matte_node->Translate(Vect (width_distrib (gen), height_distrib (gen), 0.0));
      AppendChild (matte_node);
    }
}

CollageBand::CollageBand (f64 _width, f64 _height,
                          std::vector<FilmInfo> const &_films)
  : Node ()
{
  f64 const size_factor = 0.3;
  f64 const placement_factor = 1.0 / 3.0;

  Vect const xxx = Vect (1.0, 0.0, 0.0);

  Vect const collage_center (0.0);

  std::array<Vect, 3> const positions
    = {collage_center - 1.0 * placement_factor * xxx * _width,
       collage_center,
       collage_center + 1.0 * placement_factor * xxx * _width};

  std::random_device rd;
  std::mt19937 genny (rd ());
  std::uniform_int_distribution<> elem_cnt_dist (4, 6);

  //top band - 4-ish collages
  for (Vect const &v : positions)
    { i64 ecnt = elem_cnt_dist (genny);
      fprintf (stderr, "and lo! <%ld> elements enchosen for THIS collage.\n",
               ecnt);
      Collage *collage = new Collage (ecnt, _films,
                                      size_factor * _width,
                                      size_factor * _height);
      SinuVect perky (0.25 * size_factor * _height * Vect::yaxis,
                      1.0 / (2.0 + drand48 ()), ZoftVect(),
                      2.0 * M_PI * drand48 ());
      SinuVect loping (0.35 * size_factor * _height * Vect::xaxis,  // yes, not _wid
                       1.0 / (6.0 + drand48 ()), ZoftVect(),
                       2.0 * M_PI * drand48 ());
      SumVect robert (perky, loping);
      collage -> Translate (robert);
      collage->Translate(v);
      AppendChild(collage);
    }

}

}
