#include <VideoSystem.hpp>

#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>
#include <TampVideoTerminus.hpp>

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

  assert (_uni_count <= 9);

  for (size_t i = 0; i < _uni_count; ++i)
    uniforms[i] = _unis[i];
}

VideoTexture::~VideoTexture ()
{
  fprintf (stderr, "destroying video\n");
  if (auto *system = VideoSystem::GetSystem(); system)
    system->DestroyVideo (this);

  for (szt i = 0; i < array_size (textures); ++i)
    if (bgfx::isValid(textures[i]))
      {
        bgfx::destroy (textures[i]);
        textures[i] = BGFX_INVALID_HANDLE;
      }
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

bgfx::UniformHandle const &VideoTexture::GetAdjColorUniform () const
{
  return uniforms[8];
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
  : m_upload_position {0u}
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
  m_vgr.uniforms[8] = bgfx::createUniform("u_adj_color",   bgfx::UniformType::Vec4);

  ProgramResiduals ps = CreateProgram ("video_vs.bin", "video_fs.bin", true);
  m_vgr.basic_program = ps.program;

  ps = CreateProgram ("matte_video_vs.bin", "matte_video_fs.bin", true);
  m_vgr.matte_program = ps.program;

  m_matte_pool = std::make_unique<MatteLoaderPool>();
}

VideoSystem::~VideoSystem ()
{
  bgfx::destroy (m_vgr.basic_program);
  bgfx::destroy (m_vgr.matte_program);
  for (size_t i = 0; i < array_size (m_vgr.uniforms); ++i)
    bgfx::destroy (m_vgr.uniforms[i]);

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

static void ReleaseBimgImageContainer (void *, void *_container)
{
  auto *cont = reinterpret_cast<bimg::ImageContainer *> (_container);
  bimg::imageFree (cont);
}

static void update_or_create_texture (bgfx::TextureHandle &_handle, u64 _flags, bimg::ImageContainer *_image)
{
  if (! bgfx::isValid(_handle))
    {
      _handle = bgfx::createTexture2D (_image->m_width, _image->m_height, false, 1,
                                       (bgfx::TextureFormat::Enum)_image->m_format, _flags);
    }

  bgfx::Memory const *mem = bgfx::makeRef(_image->m_data, _image->m_size, ReleaseBimgImageContainer, _image);
  bgfx::updateTexture2D(_handle, 0, 0, 0, 0, _image->m_width, _image->m_height, mem);
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

  // if (! (GST_VIDEO_INFO_IS_RGB (video_info) ||
  //        GST_VIDEO_INFO_IS_GRAY(video_info)))
  //   return;

  if (! GST_VIDEO_INFO_IS_YUV(video_info))
    return;


  width = GST_VIDEO_INFO_WIDTH (video_info);
  height = GST_VIDEO_INFO_HEIGHT (video_info);
  components = GST_VIDEO_INFO_N_COMPONENTS (video_info);
  _textures->SetDimensions({width, height});

  //RGB is all one plane
  assert (GST_VIDEO_INFO_N_PLANES(video_info) < 4);
  for (u32 i = 0; i < GST_VIDEO_INFO_N_COMPONENTS(video_info); ++i)
    {
      int comp_width = GST_VIDEO_INFO_COMP_WIDTH(video_info, i);
      int comp_height = GST_VIDEO_INFO_COMP_HEIGHT(video_info, i);
      stride = GST_VIDEO_INFO_COMP_STRIDE(video_info, i);
      int plane_idx = GST_VIDEO_INFO_COMP_PLANE(video_info, i);

      // align = calculate_alignment (stride);

      //printf ("frame: %d x %d with %d comp, %d stride\n", width, height, components, stride);

      video_frame_holder *frame_holder = new video_frame_holder (_sample, video_info);
      const bgfx::Memory *mem
        = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, plane_idx),
                         GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
                         DeleteImageLeftOvers<video_frame_holder>, frame_holder);

      bgfx::TextureHandle &text_handle = _textures->GetNthTexture(i);

      update_or_create_texture(text_handle, comp_width, comp_height, 1, stride,
                               BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_POINT, mem);
    }
}

void VideoSystem::UploadFrames ()
{
  auto do_upload = [&] (VideoPipeline &pipe, szt &score)
    {
      auto texture = pipe.texture.lock ();
      if (! texture)
        return;

      GstVideoInfo video_info;
      gst_ptr<GstSample> sample = pipe.terminus->FetchClearSample();
      if (sample)
        {
          upload_frame(sample, texture, &video_info);
          score += 3;

          if (pipe.matte_dir_path.empty())
            return;

          GstBuffer *buffer = gst_sample_get_buffer (sample.get ());
          guint64 pts = GST_BUFFER_PTS(buffer);

          guint64 frame_num = guint64 (std::round (pts * GST_VIDEO_INFO_FPS_N(&video_info)
                                                   / GST_VIDEO_INFO_FPS_D(&video_info) / f64(1e9)));
          pipe.matte_awaited = i32 (frame_num);
          //printf ("pts: %f, now: %f, for %lu\n", pts/f64(1e9), offset_ns/f64(1e9), frame_num);
        }

      if (pipe.matte_awaited >= 0)
        {
          MatteFrameUnique mf;
          if (! pipe.matte_worker->PopFrame(pipe.matte_awaited, mf))
            {
              fprintf (stdout, "no matte for %d which seems fishy\n", pipe.matte_awaited);
              return;
            }

          pipe.matte_awaited = -1;
          // bimg::ImageContainer *image = mf.data.release();
          // update_or_create_texture(texture->GetNthTexture(3),
          //                          BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, image);
          u8 *data = mf.data.release();
          bgfx::Memory const *mem = bgfx::makeRef(data, mf.data_size, BGFXfree);
          update_or_create_texture(texture->GetNthTexture(3), mf.width, mf.height,
                                   1, mf.width, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
          score += 1;
        }
    };

  auto pred_null_texture = [] (VideoPipeline const &pipe) { return pipe.texture.expired(); };

  auto const it = std::remove_if (m_pipelines.begin (), m_pipelines.end (), pred_null_texture);
//  fprintf (stderr, "distance is %ld\n", std::distance (it, m_pipelines.end ()));
  m_pipelines.erase (it, m_pipelines.end ());

  szt const max_score = 10000u;
  szt score = 0u;
  szt const count = m_pipelines.size ();
  m_upload_position = m_upload_position >= count ? 0 : m_upload_position;
  for (szt i = 0; i < count; ++i)
    {
      do_upload(m_pipelines[m_upload_position], score);
      m_upload_position = (m_upload_position + 1) % count;
      if (score >= max_score)
        break;
    }

  // fprintf (stderr, "pos: %zu, score: %zu\n", m_upload_position, score);
}

void VideoSystem::PollMessages()
{
  for (VideoPipeline &pipe : m_pipelines)
    pipe.pipeline->PollMessages();
}

VideoBrace VideoSystem::OpenVideo (std::string_view _uri)
{
  auto *video_term = new TampVideoTerminus;
  auto *audio_term = new NullTerminus ("audio/x-raw");
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  dec->OpenVideoFile (_uri, video_term, audio_term);
  dec->Play ();

  ch_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::I420, m_vgr.basic_state, m_vgr.basic_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  m_pipelines.emplace_back();
  VideoPipeline &pipe = m_pipelines.back ();
  pipe.uri = _uri;
  pipe.pipeline = dec;
  pipe.terminus = video_term;
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

      m_pipelines.erase(cur);
      break;
    }
}


VideoBrace VideoSystem::OpenMatte (std::string_view _uri,
                                   f64 _loop_start_ts, f64 _loop_end_ts,
                                   i32 _frame_count, fs::path const &_matte_dir,
                                   v2i32 _min, v2i32 _max)
{
  // set up matte worker
  m_pipelines.emplace_back();
  VideoPipeline &pipe = m_pipelines.back ();
  pipe.matte_worker = make_ch<MatteLoaderWorker> (_loop_start_ts, _frame_count, _matte_dir);
  pipe.matte_worker->SetMatteLoaderPool(m_matte_pool.get ());

  // add video terminus
  auto *term = new TampVideoTerminus;
  term->AddMatteWorker(pipe.matte_worker);

  // create media pipeline
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  dec->OpenVideoFile (_uri, term);
  dec->Play ();
  dec->Loop (_loop_start_ts, _loop_end_ts);

  ch_ptr<VideoTexture>
    txt{new VideoTexture (VideoFormat::I420, m_vgr.matte_state, m_vgr.matte_program,
                          m_vgr.uniforms, array_size (m_vgr.uniforms))};

  txt->SetMatteDimensions(_min, _max);

  pipe.uri = _uri;
  pipe.pipeline = dec;
  pipe.terminus = term;
  pipe.texture = txt;
  pipe.adjusted_loop_start_ts = -1;
  pipe.adjusted_loop_end_ts = -1;
  pipe.matte_frame_count = _frame_count;
  pipe.matte_dir_path = _matte_dir;

  return {dec, txt};
}


}
