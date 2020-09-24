#include <MattedVideoRenderable.hpp>

#include <BlockTimer.hpp>
#include <Node.hpp>

#include <algorithm>

namespace charm
{

namespace fs = std::filesystem;
MattedVideoRenderable::MattedVideoRenderable (std::string_view _uri,
                                              f64 _loop_start_ts, f64 _loop_end_ts,
                                              std::filesystem::path const &_matte_dir)
  : Renderable (),
    m_program {BGFX_INVALID_HANDLE},
    m_texture {BGFX_INVALID_HANDLE},
    m_matte {BGFX_INVALID_HANDLE},
    m_uni_vid_texture {BGFX_INVALID_HANDLE},
    m_uni_matte_texture {BGFX_INVALID_HANDLE},
    m_video_pipeline {nullptr},
    m_terminus {nullptr},
    m_aspect_ratio {1.0f}
{
  m_uni_vid_texture = bgfx::createUniform("u_video_texture", bgfx::UniformType::Sampler);
  m_uni_matte_texture = bgfx::createUniform("u_video_matte", bgfx::UniformType::Sampler);
  m_uni_aspect_ratio = bgfx::createUniform("u_aspect_ratio", bgfx::UniformType::Vec4);

  ProgramResiduals ps = CreateProgram ("matte_video_vs.bin", "matte_video_fs.bin", true);
  m_program = ps.program;

  m_terminus = new BasicPipelineTerminus (false);
  m_video_pipeline = new DecodePipeline;
  m_video_pipeline->Open (_uri, m_terminus);
  {
    BlockTimer bt ("loop change");
    m_video_pipeline->Play ();
    m_video_pipeline->Loop (_loop_start_ts, _loop_end_ts);
   }

  m_matte_path = _matte_dir;

  m_mattes.insert (m_mattes.end (),
                   fs::directory_iterator (m_matte_path),
                   fs::directory_iterator ());

  std::sort (m_mattes.begin (), m_mattes.end ());
}

MattedVideoRenderable::~MattedVideoRenderable ()
{
  bgfx::destroy (m_uni_aspect_ratio);
  bgfx::destroy (m_uni_matte_texture);
  bgfx::destroy (m_uni_vid_texture);
  bgfx::destroy (m_program);
  bgfx::destroy (m_matte);
  bgfx::destroy (m_texture);

  delete m_video_pipeline;
  m_terminus = nullptr;
}

void MattedVideoRenderable::UploadSample (gst_ptr<GstSample> const &_sample)
{
  GstCaps *sample_caps = gst_sample_get_caps(_sample.get ());
  GstVideoInfo video_info;
  int width, height, components, stride;

  if (! sample_caps)
    return;

  gst_video_info_init(&video_info);
  if (! gst_video_info_from_caps (&video_info, sample_caps))
    return;

  if (! GST_VIDEO_INFO_IS_RGB (&video_info))
    return;

  width = GST_VIDEO_INFO_WIDTH (&video_info);
  height = GST_VIDEO_INFO_HEIGHT (&video_info);
  components = GST_VIDEO_INFO_N_COMPONENTS (&video_info);
  //RGB is all one plane
  stride = GST_VIDEO_INFO_PLANE_STRIDE(&video_info, 0);
  // align = calculate_alignment (stride);
  (void)components;

  m_aspect_ratio = width / (f32)height;

  if (! bgfx::isValid(m_texture))
    {
      fprintf (stderr, "create video texture\n");
      bgfx::TextureFormat::Enum const formats[5]
        { bgfx::TextureFormat::Unknown, //shouldn't happen
          bgfx::TextureFormat::R8,
          bgfx::TextureFormat::RG8,
          bgfx::TextureFormat::RGB8,
          bgfx::TextureFormat::RGBA8 };

      m_texture = bgfx::createTexture2D(width, height, false, 1,
                                        formats[components],
                                        BGFX_SAMPLER_UVW_CLAMP |
                                        BGFX_SAMPLER_POINT);
    }

  video_frame_holder *frame_holder = new video_frame_holder (_sample, &video_info);

  const bgfx::Memory *mem = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, 0),
                                           GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
                                           DeleteImageLeftOvers<video_frame_holder>, frame_holder);

  bgfx::updateTexture2D(m_texture, 0, 0, 0, 0, width, height, mem, stride);


  GstBuffer *buffer = gst_sample_get_buffer (_sample.get ());
  guint64 pts = GST_BUFFER_PTS(buffer);

  if (gint64(pts) < m_video_pipeline->m_loop_status.loop_start ||
      gint64(pts) > m_video_pipeline->m_loop_status.loop_end)
    return;

  //i expect looping here for now
  guint64 offset_ns = pts - m_video_pipeline->m_loop_status.loop_start;
  guint64 frame_num = guint64 (offset_ns * GST_VIDEO_INFO_FPS_N(&video_info)
                               / (GST_VIDEO_INFO_FPS_D(&video_info) * f64(1e9)));
  printf ("pts: %.3f, now: %.3f\n", pts/f64(1e9), offset_ns/f64(1e9));

  auto &matte = m_mattes[frame_num];
  assert (frame_num < m_mattes.size ());

  (void)matte;
  if (bgfx::isValid(m_matte))
    bgfx::destroy (m_matte);
  m_matte = LoadTexture2D (matte.path().c_str(), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
  if (! bgfx::isValid(m_matte))
    fprintf (stderr, "returned handle isn't valid\n");
}

void MattedVideoRenderable::Update ()
{
  if (m_video_pipeline)
    {
      m_video_pipeline->PollMessages ();
      gst_ptr<GstSample> new_sample = m_terminus->FetchClearSample();

      if (new_sample)
        UploadSample(new_sample);
    }
}

void MattedVideoRenderable::Draw ()
{
  if (! bgfx::isValid(m_texture))
    return;

  u64 const state =
    BGFX_STATE_WRITE_RGB |
    BGFX_STATE_WRITE_A     |
    BGFX_STATE_PT_TRISTRIP |
    BGFX_STATE_WRITE_Z     |
    BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);
  bgfx::setState(state);
  bgfx::setVertexCount(4);

  bgfx::setTexture(0, m_uni_vid_texture, m_texture);
  bgfx::setTexture(1, m_uni_matte_texture, m_matte);

  glm::vec4 unity {1.0f, 1.0f/m_aspect_ratio, 1.0f, 1.0f};
  bgfx::setUniform(m_uni_aspect_ratio, glm::value_ptr (unity));

  bgfx::submit(0, m_program);
}

}
