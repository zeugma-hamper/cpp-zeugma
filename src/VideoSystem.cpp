#include <VideoSystem.hpp>

#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

namespace charm
{
static VideoSystem *s_video_system_instance{nullptr};

template<typename T, szt N>
constexpr szt array_size (T const (&)[N])
{
  return N;
}

VideoTexture::VideoTexture (VideoFormat _format, u64 _state, bgfx::ProgramHandle _program,
                            bgfx::UniformHandle *_unis, size_t _uni_count)
  : format {_format},
    default_state {_state},
    program {_program}
{
  for (size_t i = 0; i < array_size (uniforms); ++i)
    uniforms[i] = BGFX_INVALID_HANDLE;

  for (size_t i = 0; i < array_size (textures); ++i)
    textures[i] = BGFX_INVALID_HANDLE;

  assert (_uni_count <= 5);

  for (size_t i = 0; i < _uni_count; ++i)
    uniforms[i] = _unis[i];
}

VideoTexture::~VideoTexture ()
{
  VideoSystem::GetSystem ()->DestroyVideo (this);
}

u64 VideoTexture::GetDefaultState () const
{
  return default_state;
}

void VideoTexture::BindGraphics (u64 _additional_state)
{
  bgfx::setState (default_state | _additional_state);

  if (format == VideoFormat::RGB)
    {
      bgfx::setTexture (0, uniforms[0], textures[0]);
    }
  else if (format == VideoFormat::I420)
    {
      bgfx::setTexture (0, uniforms[0], textures[0]);
      bgfx::setTexture (1, uniforms[0], textures[0]);
      bgfx::setTexture (2, uniforms[0], textures[0]);
    }

  // if matte is valid
  if (bgfx::isValid(textures[3]))
    bgfx::setTexture (3, uniforms[3], textures[3]);
}

void VideoTexture::SetNthTexture (size_t _index, bgfx::TextureHandle _handle)
{
  assert (_index < array_size (textures));

  if (bgfx::isValid(textures[_index]))
    bgfx::destroy (textures[_index]);

  textures[_index] = _handle;
}

bgfx::TextureHandle VideoTexture::GetNthTexture (size_t _index) const
{
  if (_index >= array_size (textures))
    return BGFX_INVALID_HANDLE;

  return textures[_index];
}

bgfx::UniformHandle const &VideoTexture::GetAspectUniform () const
{
  return uniforms[4];
}

bgfx::ProgramHandle const &VideoTexture::GetProgram () const
{
  return program;
}

bool VideoSystem::Initialize ()
{
  s_video_system_instance = new VideoSystem;
  return true;
}

void VideoSystem::ShutDown ()
{
  delete s_video_system_instance;
}

VideoSystem *VideoSystem::GetSystem ()
{
  return s_video_system_instance;
}


VideoSystem::VideoSystem ()
{
  gst_init (nullptr, nullptr);

  m_pipelines.reserve (24);

  m_vgr.uniforms[0] = bgfx::createUniform("u_video_texture0", bgfx::UniformType::Sampler);
  m_vgr.uniforms[1] = bgfx::createUniform("u_video_texture1", bgfx::UniformType::Sampler);
  m_vgr.uniforms[2] = bgfx::createUniform("u_video_texture2", bgfx::UniformType::Sampler);
  m_vgr.uniforms[3] = bgfx::createUniform("u_video_matte",    bgfx::UniformType::Sampler);
  m_vgr.uniforms[4] = bgfx::createUniform("u_aspect_ratio", bgfx::UniformType::Vec4);

  ProgramResiduals ps = CreateProgram ("video_vs.bin", "video_fs.bin", true);
  m_vgr.basic_program = ps.program;

  ps = CreateProgram ("matte_video_vs.bin", "matte_video_fs.bin", true);
  m_vgr.matte_program = ps.program;
}

VideoSystem::~VideoSystem ()
{
  bgfx::destroy (m_vgr.basic_program);
  bgfx::destroy (m_vgr.matte_program);
  for (size_t i = 0; i < array_size (m_vgr.uniforms); ++i)
    bgfx::destroy (m_vgr.uniforms[i]);

  for (VideoPipeline &pipe : m_pipelines)
    pipe.pipeline->SetState (GST_STATE_NULL);

  for (VideoPipeline &pipe : m_pipelines)
    delete pipe.pipeline;

  m_pipelines.clear ();
}

void VideoSystem::UploadFrames ()
{
  for (VideoPipeline &pipe : m_pipelines)
    {
      gst_ptr<GstSample> sample = pipe.terminus->FetchClearSample();
      if (! sample)
        continue;

      fprintf (stderr, "uploading a sample\n");
      GstCaps *sample_caps = gst_sample_get_caps(sample.get ());
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

      auto texture = pipe.texture.lock ();
      bgfx::TextureHandle rgb_handle = texture->GetNthTexture(0);
      if (! bgfx::isValid(rgb_handle))
        {
          fprintf (stderr, "create video texture\n");
          bgfx::TextureFormat::Enum const formats[5]
            { bgfx::TextureFormat::Unknown, //shouldn't happen
              bgfx::TextureFormat::R8,
              bgfx::TextureFormat::RG8,
              bgfx::TextureFormat::RGB8,
              bgfx::TextureFormat::RGBA8 };

          rgb_handle = bgfx::createTexture2D(width, height, false, 1,
                                             formats[components],
                                             BGFX_SAMPLER_UVW_CLAMP |
                                             BGFX_SAMPLER_POINT);
          texture->SetNthTexture(0, rgb_handle);
        }

      video_frame_holder *frame_holder = new video_frame_holder (sample, &video_info);

      const bgfx::Memory *mem
        = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, 0),
                         GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
                         DeleteImageLeftOvers<video_frame_holder>, frame_holder);

      bgfx::updateTexture2D(rgb_handle, 0, 0, 0, 0, width, height, mem, stride);
    }
}

void VideoSystem::PollMessages()
{
  for (VideoPipeline &pipe : m_pipelines)
    pipe.pipeline->PollMessages();
}

intrusive_ptr<VideoTexture> VideoSystem::OpenVideo (std::string_view _uri)
{
  BasicPipelineTerminus *term = new BasicPipelineTerminus (false);
  DecodePipeline *dec = new DecodePipeline;
  dec->Open (_uri, term);
  dec->Play ();

  intrusive_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::RGB, m_vgr.basic_state, m_vgr.basic_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  m_pipelines.push_back (VideoPipeline{std::string (_uri), dec, term,
                                       intrusive_weak_ptr{txt}});

  return txt;
}

//TODO: change this to not mutate vector of pipelines
void VideoSystem::DestroyVideo (VideoTexture *_texture)
{
  auto const pend = m_pipelines.end ();
  for (auto cur = m_pipelines.begin (); cur != pend; ++cur)
    {
      if (cur->texture != _texture)
        continue;

      for (szt i = 0; i < 4; ++i)
        _texture->SetNthTexture(0, BGFX_INVALID_HANDLE);

      cur->pipeline->SetState(GST_STATE_NULL);
      delete cur->pipeline;
      m_pipelines.erase(cur);
      break;
    }
}


intrusive_ptr<VideoTexture> VideoSystem::OpenMatte (std::string_view _uri,
                                                    f64 _loop_start_ts, f64 _loop_end_ts,
                                                    std::filesystem::path const &_matte_dir)
{
  BasicPipelineTerminus *term = new BasicPipelineTerminus (false);
  DecodePipeline *dec = new DecodePipeline;
  dec->Open (_uri, term);
  dec->Play ();

  intrusive_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::RGB, m_vgr.basic_state, m_vgr.basic_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  m_pipelines.push_back (VideoPipeline{std::string (_uri), dec, term,
                                       intrusive_weak_ptr{txt}});

  return txt;
}


}
