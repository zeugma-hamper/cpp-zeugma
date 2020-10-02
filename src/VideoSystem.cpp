#include <VideoSystem.hpp>

#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <algorithm>

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
      bgfx::setTexture (1, uniforms[1], textures[1]);
      bgfx::setTexture (2, uniforms[2], textures[2]);
    }

  // if matte is valid
  if (bgfx::isValid(textures[3]))
    {
      //fprintf (stderr, "binding matte\n");
      bgfx::setTexture (3, uniforms[3], textures[3]);
    }
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

      if (pipe.matte_dir_path.empty())
        continue;

      GstBuffer *buffer = gst_sample_get_buffer (sample.get ());
      guint64 pts = GST_BUFFER_PTS(buffer);

      //TODO: set matte texture to pass through
      if (gint64(pts) < pipe.pipeline->m_loop_status.loop_start ||
          gint64(pts) > pipe.pipeline->m_loop_status.loop_end)
        return;

      //i expect looping here for now
      guint64 offset_ns = pts - pipe.pipeline->m_loop_status.loop_start;
      guint64 frame_num = guint64 (offset_ns * GST_VIDEO_INFO_FPS_N(&video_info)
                                   / (GST_VIDEO_INFO_FPS_D(&video_info) * f64(1e9)));
      //printf ("pts: %.3f, now: %.3f\n", pts/f64(1e9), offset_ns/f64(1e9));

      assert (frame_num < pipe.matte_file_paths.size ());
      auto &matte = pipe.matte_file_paths[frame_num];

      (void)matte;
      //3 is dedicated matte texture
      if (! bgfx::isValid (texture->GetNthTexture(3)))
        {
          bgfx::TextureHandle txt_matte = CreateTexture2D (matte.path().c_str(),
                                                           BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
          if (! bgfx::isValid (txt_matte))
            fprintf (stderr, "returned matte handle isn't valid\n");

          texture->SetNthTexture(3, txt_matte);
        }
      else
        {
          UpdateWholeTexture2D(texture->GetNthTexture (3), matte.path().c_str());
        }

    }
}

void VideoSystem::PollMessages()
{
  for (VideoPipeline &pipe : m_pipelines)
    pipe.pipeline->PollMessages();
}

VideoBrace VideoSystem::OpenVideo (std::string_view _uri)
{
  BasicPipelineTerminus *term = new BasicPipelineTerminus (false);
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  dec->Open (_uri, term);
  dec->Play ();

  ch_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::RGB, m_vgr.basic_state, m_vgr.basic_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  m_pipelines.emplace_back();
  VideoPipeline &pipe = m_pipelines.back ();
  pipe.uri = _uri;
  pipe.pipeline = dec;
  pipe.terminus = term;
  pipe.texture = txt;

  return {dec, txt};
}

ch_ptr<DecodePipeline>
VideoSystem::FindDecodePipeline (ch_ptr<VideoTexture> const &_texture)
{
  for (auto cur : m_pipelines)
    {
      if (_texture == cur.texture)
        return cur.pipeline;
    }

  return {};
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
      m_pipelines.erase(cur);
      break;
    }
}


VideoBrace VideoSystem::OpenMatte (std::string_view _uri,
                                   f64 _loop_start_ts, f64 _loop_end_ts,
                                   std::filesystem::path const &_matte_dir)
{
  BasicPipelineTerminus *term = new BasicPipelineTerminus (false);
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  dec->Open (_uri, term);
  dec->Play ();
  dec->Loop (_loop_start_ts, _loop_end_ts);

  ch_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::RGB, m_vgr.matte_state, m_vgr.matte_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  m_pipelines.emplace_back();
  VideoPipeline &pipe = m_pipelines.back ();
  pipe.uri = _uri;
  pipe.pipeline = dec;
  pipe.terminus = term;
  pipe.texture = txt;
  pipe.loop_start_ts = _loop_start_ts;
  pipe.loop_end_ts = _loop_end_ts;
  pipe.matte_dir_path = _matte_dir;

  pipe.matte_file_paths.insert (pipe.matte_file_paths.end (),
                                fs::directory_iterator (_matte_dir),
                                fs::directory_iterator ());

  std::sort (pipe.matte_file_paths.begin (), pipe.matte_file_paths.end ());

  return {dec, txt};
}


}
