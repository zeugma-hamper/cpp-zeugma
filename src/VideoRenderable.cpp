#include <VideoRenderable.hpp>

#include <Node.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace charm
{

VideoRenderable::VideoRenderable (std::string_view _uri)
  : Renderable (),
    m_program {BGFX_INVALID_HANDLE},
    m_texture {BGFX_INVALID_HANDLE},
    m_uni_vid_texture {BGFX_INVALID_HANDLE},
    m_video_pipeline {nullptr},
    m_terminus {nullptr}
{
  m_uni_vid_texture = bgfx::createUniform("u_video_texture", bgfx::UniformType::Sampler);
  m_uni_aspect_ratio = bgfx::createUniform("u_aspect_ratio", bgfx::UniformType::Vec4);

  // see compile_shader.sh in project root
  ProgramResiduals ps = CreateProgram ("video_vs.bin", "video_fs.bin", true);
  m_program = ps.program;

  m_terminus = new BasicPipelineTerminus (false);
  m_video_pipeline = new DecodePipeline;
  m_video_pipeline->Open (_uri, m_terminus);
  m_video_pipeline->Play();
}

VideoRenderable::~VideoRenderable ()
{
  bgfx::destroy (m_uni_aspect_ratio);
  bgfx::destroy (m_uni_vid_texture);
  bgfx::destroy (m_program);
  bgfx::destroy (m_texture);

  delete m_video_pipeline;
  m_terminus = nullptr;
}

struct video_frame_holder
{
  gst_ptr<GstSample> sample;
  GstVideoFrame video_frame;

  video_frame_holder (gst_ptr<GstSample> const &_sample, GstVideoInfo *_info)
    : sample {_sample}
  {
    GstBuffer *buffer = gst_sample_get_buffer(_sample.get ());
    gst_video_frame_map (&video_frame, _info, buffer, GST_MAP_READ);
  }

  ~video_frame_holder ()
  {
    gst_video_frame_unmap(&video_frame);
  }
};

static void free_video_holder (void *, void *_user_data)
{
  video_frame_holder *holder = (video_frame_holder *)_user_data;
  delete holder;
}

void VideoRenderable::UploadSample (gst_ptr<GstSample> const &_sample)
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
                                           free_video_holder, frame_holder);

  bgfx::updateTexture2D(m_texture, 0, 0, 0, 0, width, height, mem, stride);
}

void VideoRenderable::Update ()
{
  if (m_video_pipeline)
    {
      m_video_pipeline->PollMessages ();
      gst_ptr<GstSample> new_sample = m_terminus->FetchClearSample();

      if (new_sample)
        UploadSample(new_sample);
    }
}

void VideoRenderable::Draw ()
{
  if (! bgfx::isValid(m_texture))
    return;

  //later: BGFX_STATE_WRITE_A |
  //       BGFX_ uhhh... blending
  u64 const state = BGFX_STATE_WRITE_RGB |
    BGFX_STATE_PT_TRISTRIP |
    BGFX_STATE_WRITE_Z;

  bgfx::setTransform(&m_node->GetAbsoluteModelTransformation());
  bgfx::setState(state);
  bgfx::setVertexCount(4);
  bgfx::setTexture(0, m_uni_vid_texture, m_texture);
  glm::vec4 unity {1.0f};
  unity.y = 1080.0f/1920.0f;
  bgfx::setUniform(m_uni_aspect_ratio, glm::value_ptr (unity));
  bgfx::submit(0, m_program);
}

}
