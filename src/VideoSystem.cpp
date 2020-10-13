#include <VideoSystem.hpp>

#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>

#include <BlockTimer.hpp>

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

  assert (_uni_count <= 8);

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

void VideoTexture::SetDimensions (v2i32 _dim)
{
  dimensions = _dim;
}

v2i32 VideoTexture::GetDimensions () const
{
  return dimensions;
}

void VideoTexture::SetMatteDimensions (v2i32 _min, v2i32 _max)
{
  matte_min = _min;
  matte_max = _max;
}

v2i32 VideoTexture::GetMatteMin () const
{
  return matte_min;
}

v2i32 VideoTexture::GetMatteMax () const
{
  return matte_max;
}

void VideoTexture::SetNthTexture (size_t _index, bgfx::TextureHandle _handle)
{
  assert (_index < array_size (textures));

  if (bgfx::isValid(textures[_index]))
    bgfx::destroy (textures[_index]);

  textures[_index] = _handle;
}

bgfx::TextureHandle &VideoTexture::GetNthTexture (size_t _index)
{
  //TODO: remove this
  static bgfx::TextureHandle placeholder;
  if (_index >= array_size (textures))
    return placeholder;

  return textures[_index];
}

bgfx::TextureHandle VideoTexture::GetNthTexture (size_t _index) const
{
  if (_index >= array_size (textures))
    return BGFX_INVALID_HANDLE;

  return textures[_index];
}


bgfx::UniformHandle const &VideoTexture::GetDimensionUniform () const
{
  return uniforms[4];
}

bgfx::UniformHandle const &VideoTexture::GetMatteDimUniform () const
{
  return uniforms[5];
}

bgfx::UniformHandle const &VideoTexture::GetOverUniform () const
{
  return uniforms[6];
}

bgfx::UniformHandle const &VideoTexture::GetUpUniform () const
{
  return uniforms[7];
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
  m_vgr.uniforms[4] = bgfx::createUniform("u_dimensions",     bgfx::UniformType::Vec4);
  m_vgr.uniforms[5] = bgfx::createUniform("u_matte_dimensions", bgfx::UniformType::Vec4);
  m_vgr.uniforms[6] = bgfx::createUniform("u_over", bgfx::UniformType::Vec4);
  m_vgr.uniforms[7] = bgfx::createUniform("u_up",   bgfx::UniformType::Vec4);

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

static void update_or_create_texture (bgfx::TextureHandle &_handle, u16 _width, u16 _height,
                                      u16 n_comp, u16 _stride, u64 _flags, bgfx::Memory const *_mem)
{
  bgfx::TextureFormat::Enum const formats[5]
    { bgfx::TextureFormat::Unknown, //shouldn't happen
      bgfx::TextureFormat::R8,
      bgfx::TextureFormat::RG8,
      bgfx::TextureFormat::RGB8,
      bgfx::TextureFormat::RGBA8 };

  if (! bgfx::isValid(_handle))
    _handle = bgfx::createTexture2D(_width, _height, false, 1, formats[n_comp], _flags);

  bgfx::updateTexture2D(_handle, 0, 0, 0, 0, _width, _height, _mem, _stride);
}

static void upload_frame (gst_ptr<GstSample> const &_sample,
                          ch_ptr<VideoTexture> const &_textures,
                          GstVideoInfo *_video_info)
{
  GstCaps *sample_caps = gst_sample_get_caps(_sample.get ());
  GstVideoInfo stack_video_info;
  GstVideoInfo *video_info = _video_info ? _video_info : &stack_video_info;
  int width, height, components, stride;

  if (! sample_caps)
    return;

  gst_video_info_init(video_info);
  if (! gst_video_info_from_caps (video_info, sample_caps))
    return;

  if (! (GST_VIDEO_INFO_IS_RGB (video_info) ||
         GST_VIDEO_INFO_IS_GRAY(video_info)))
    return;

  width = GST_VIDEO_INFO_WIDTH (video_info);
  height = GST_VIDEO_INFO_HEIGHT (video_info);
  components = GST_VIDEO_INFO_N_COMPONENTS (video_info);
  //RGB is all one plane
  stride = GST_VIDEO_INFO_PLANE_STRIDE(video_info, 0);
  // align = calculate_alignment (stride);

  //printf ("frame: %d x %d with %d comp, %d stride\n", width, height, components, stride);

  _textures->SetDimensions({width, height});

  video_frame_holder *frame_holder = new video_frame_holder (_sample, video_info);
  const bgfx::Memory *mem
    = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, 0),
                     GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
                     DeleteImageLeftOvers<video_frame_holder>, frame_holder);

  bgfx::TextureHandle &text_handle = _textures->GetNthTexture(0);

  update_or_create_texture(text_handle, width, height, components, stride,
                           BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_POINT, mem);
}

void VideoSystem::UploadFrames ()
{
  for (VideoPipeline &pipe : m_pipelines)
    {
      GstVideoInfo video_info;
      gst_ptr<GstSample> sample = pipe.terminus->FetchClearSample();
      if (! sample)
        continue;

      auto texture = pipe.texture.lock ();
      if (! texture)
        continue;

      upload_frame(sample, texture, &video_info);

      if (pipe.matte_dir_path.empty())
        continue;

      GstBuffer *buffer = gst_sample_get_buffer (sample.get ());
      guint64 pts = GST_BUFFER_PTS(buffer);

      auto calc_dur = [&video_info] (gint64 n_frames) -> gint64
      { return n_frames * GST_VIDEO_INFO_FPS_D(&video_info) * 1e9
          / GST_VIDEO_INFO_FPS_N(&video_info); };

      gint64 const frame_dur = calc_dur (1);
      if (pipe.adjusted_loop_start_ts == -1 &&
          pipe.pipeline->m_loop_status.loop_start <= i64 (pts) &&
          i64 (pts) <= pipe.pipeline->m_loop_status.loop_start + frame_dur)
        {
          pipe.adjusted_loop_start_ts = i64 (pts);
          pipe.adjusted_loop_end_ts
            = pipe.adjusted_loop_start_ts + calc_dur (pipe.matte_frame_count);
        }

      //TODO: set matte texture to pass through
      if (gint64(pts) < pipe.adjusted_loop_start_ts ||
          gint64(pts) > pipe.adjusted_loop_end_ts + (frame_dur / 2))
        continue;

      //i expect looping here for now
      guint64 offset_ns = pts - pipe.adjusted_loop_start_ts;
      guint64 frame_num = guint64 (std::round (offset_ns * GST_VIDEO_INFO_FPS_N(&video_info)
                                               / GST_VIDEO_INFO_FPS_D(&video_info) / f64(1e9)));
      //printf ("pts: %f, now: %f, for %lu\n", pts/f64(1e9), offset_ns/f64(1e9), frame_num);

      MatteFrame mf = pipe.matte_loader->GetFrame(frame_num);
      if (! mf.data)
        {
          fprintf (stderr, "no matte for %lu which seems fishy\n", frame_num);
          continue;
        }

      bgfx::Memory const *memory
        = bgfx::makeRef(mf.data, mf.data_size, MatteLoader::MatteDataReleaseFn);

      bgfx::TextureHandle &matte_texture = texture->GetNthTexture(3);
      if (! bgfx::isValid(matte_texture))
        matte_texture = bgfx::createTexture2D(mf.width, mf.height, false, 1, mf.format);

      bgfx::updateTexture2D(matte_texture, 0, 0, 0, 0, mf.width, mf.height, memory);
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
  dec->OpenVideoFile (_uri, term);
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
  for (VideoPipeline &pipe : m_pipelines)
    {
      if (_texture == pipe.texture)
        return pipe.pipeline;
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
                                   i32 _frame_count, fs::path const &_matte_dir,
                                   v2i32 _min, v2i32 _max)
{
  BasicPipelineTerminus *term = new BasicPipelineTerminus (false);
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  dec->OpenVideoFile (_uri, term);
  dec->Play ();
  dec->Loop (_loop_start_ts, _loop_end_ts);

  ch_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::RGB, m_vgr.matte_state, m_vgr.matte_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  txt->SetMatteDimensions(_min, _max);

  m_pipelines.emplace_back();
  VideoPipeline &pipe = m_pipelines.back ();
  pipe.uri = _uri;
  pipe.pipeline = dec;
  pipe.terminus = term;
  pipe.texture = txt;
  pipe.adjusted_loop_start_ts = -1;
  pipe.adjusted_loop_end_ts = -1;
  pipe.matte_frame_count = _frame_count;
  pipe.matte_loader = std::make_unique<MatteLoader> ();
  pipe.matte_loader->StartLoading (_matte_dir);
  pipe.matte_dir_path = _matte_dir;

  return {dec, txt};
}


}
