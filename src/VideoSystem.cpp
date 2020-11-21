#include <VideoSystem.hpp>

#include <VideoRenderable.hpp>
#include <MattedVideoRenderable.hpp>

#include <DecodePipeline.hpp>
#include <PipelineTerminus.hpp>
#include <TampVideoTerminus.hpp>

#include <BlockTimer.hpp>
#include <vector_utils.hpp>

#include <algorithm>

namespace charm
{
static VideoSystem *s_video_system_instance{nullptr};

VideoTexture::VideoTexture (VideoFormat _format, bgfx::ProgramHandle _program,
                            bgfx::UniformHandle *_unis, size_t _uni_count)
  : format {_format},
    program {_program}
{
  for (size_t i = 0; i < ArraySize (uniforms); ++i)
    uniforms[i] = BGFX_INVALID_HANDLE;

  for (size_t i = 0; i < ArraySize (textures); ++i)
    textures[i] = BGFX_INVALID_HANDLE;

  assert (_uni_count <= 9);

  for (size_t i = 0; i < _uni_count; ++i)
    uniforms[i] = _unis[i];
}

VideoTexture::~VideoTexture ()
{
  fprintf (stderr, "destroying video\n");
  // if (auto *system = VideoSystem::GetSystem(); system)
  //   system->DestroyVideo (this);

  for (szt i = 0; i < ArraySize (textures); ++i)
    if (bgfx::isValid(textures[i]))
      {
        bgfx::destroy (textures[i]);
        textures[i] = BGFX_INVALID_HANDLE;
      }
}

void VideoTexture::BindGraphics (u64 _additional_state, bool _use_matte)
{
  bgfx::setState (_additional_state);

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
      if (_use_matte)
        bgfx::setTexture (3, uniforms[3], textures[3]);
      else
        {
          bgfx::TextureHandle const bl = VideoSystem::GetSystem()->GetBlackTexture();
          bgfx::setTexture (3, uniforms[3], bl);
        }
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
  assert (_index < ArraySize (textures));

  if (bgfx::isValid(textures[_index]))
    bgfx::destroy (textures[_index]);

  textures[_index] = _handle;
}

bgfx::TextureHandle &VideoTexture::GetNthTexture (size_t _index)
{
  //TODO: remove this
  static bgfx::TextureHandle placeholder;
  if (_index >= ArraySize (textures))
    return placeholder;

  return textures[_index];
}

bgfx::TextureHandle VideoTexture::GetNthTexture (size_t _index) const
{
  if (_index >= ArraySize (textures))
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

bgfx::UniformHandle const &VideoTexture::GetFlagsUniform () const
{
  return uniforms[8];
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


///////////////// VideoPipeline

VideoPipeline::VideoPipeline ()
  : terminus {nullptr},
    active_matte {0}
{
}

VideoPipeline::~VideoPipeline ()
{
}

ch_ptr<VideoTexture> VideoPipeline::OpenFile (std::string_view _path)
{
  auto *system = VideoSystem::GetSystem ();
  pipeline = system->CreateDecodePipeline(std::string{"file://"}.append(_path));

  if (! pipeline)
    return {};

  ch_ptr<VideoTexture> ret = system->CreateVideoTexture (VideoFormat::I420);
  texture = ret;

  pipeline->Play ();

  path = _path;
  terminus = dynamic_cast<TampVideoTerminus *> (pipeline->m_video_terminus.get ());

  return ret;
}

bool VideoPipeline::AddMatte (f64 _loop_start_ts, f64 _loop_end_ts,
                              i32 _frame_count, fs::path const &_matte_dir,
                              v2i32 _min, v2i32 _max)
{
  auto *system = VideoSystem::GetSystem();
  assert (system);
  MattePipeline pipe
    = system->CreateMattePipeline(_loop_start_ts, _loop_end_ts,
                                  _frame_count, _matte_dir, _min, _max);
  terminus->AddMatteWorker(pipe.worker);

  if (mattes.size () == 0)
    active_matte = 0;

  mattes.push_back(std::move (pipe));

  return true;
}

ch_ptr<DecodePipeline> const &VideoPipeline::GetDecoder ()
{
  return pipeline;
}

ch_ptr<VideoTexture> VideoPipeline::GetVideoTexture ()
{
  return texture.lock ();
}

i64 VideoPipeline::MatteCount () const
{
  return i64 (mattes.size ());
}

MattePipeline &VideoPipeline::NthMatte (i64 _nth)
{
  assert (_nth < MatteCount());
  return mattes[_nth];
}


void VideoPipeline::SetActiveMatte (i64 _nth)
{
  if (active_matte == _nth)
    return;

  ch_ptr<VideoTexture> text = GetVideoTexture ();
  if (! text)
    return;

  active_matte = _nth;
}

void VideoPipeline::ClearMattes ()
{
  i64 const curr_matte = active_matte;
  if (curr_matte < 0)
    return;

  current_matte = -1;
  active_matte = -1;
  mattes.clear();
  ch_ptr<VideoTexture> text = GetVideoTexture ();
  if (! text)
    return;

  bgfx::Memory const *mem = bgfx::alloc(4);
  memset (mem->data, 0, 4);
  bgfx::TextureHandle handle = bgfx::createTexture2D(2, 2, false, 1, bgfx::TextureFormat::R8);
  text->SetNthTexture(3, handle);
  return;
}



///////////////// VideoSystem

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
  m_vgr.uniforms[8] = bgfx::createUniform("u_flags",   bgfx::UniformType::Vec4);

  ProgramResiduals ps = CreateProgram ("video_vs.bin", "video_fs.bin", true);
  m_vgr.basic_program = ps.program;

  ps = CreateProgram ("matte_video_vs.bin", "matte_video_fs.bin", true);
  m_vgr.matte_program = ps.program;

  m_matte_pool = std::make_unique<MatteLoaderPool>();

  bgfx::Memory const *bl_mem = bgfx::alloc (4);
  for (i32 i = 0; i < 4; ++i)
    bl_mem->data[i] = 0;

  m_vgr.black_texture = bgfx::createTexture2D (2, 2, false, 1, bgfx::TextureFormat::R8,
                                               DefaultTextureFlags, bl_mem);

  bgfx::Memory const *wh_mem = bgfx::alloc (4);
  for (i32 i = 0; i < 4; ++i)
    wh_mem->data[i] = 255u;

  m_vgr.white_texture = bgfx::createTexture2D (2, 2, false, 1, bgfx::TextureFormat::R8,
                                               DefaultTextureFlags, wh_mem);
}

VideoSystem::~VideoSystem ()
{
  bgfx::destroy (m_vgr.basic_program);
  bgfx::destroy (m_vgr.matte_program);
  for (size_t i = 0; i < ArraySize (m_vgr.uniforms); ++i)
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

// static void ReleaseBimgImageContainer (void *, void *_container)
// {
//   auto *cont = reinterpret_cast<bimg::ImageContainer *> (_container);
//   bimg::imageFree (cont);
// }

// TODO retry compressed textures later
// static void update_or_create_texture (bgfx::TextureHandle &_handle, u64 _flags, bimg::ImageContainer *_image)
// {
//   if (! bgfx::isValid(_handle))
//     {
//       _handle = bgfx::createTexture2D (_image->m_width, _image->m_height, false, 1,
//                                        (bgfx::TextureFormat::Enum)_image->m_format, _flags);
//     }

//   bgfx::Memory const *mem = bgfx::makeRef(_image->m_data, _image->m_size, ReleaseBimgImageContainer, _image);
//   bgfx::updateTexture2D(_handle, 0, 0, 0, 0, _image->m_width, _image->m_height, mem);
// }

static void upload_frame (gst_ptr<GstSample> const &_sample,
                          ch_ptr<VideoTexture> const &_textures,
                          GstVideoInfo *_video_info)
{
  GstCaps *sample_caps = gst_sample_get_caps(_sample.get ());
  GstVideoInfo stack_video_info;
  GstVideoInfo *video_info = _video_info ? _video_info : &stack_video_info;
  int width, height, stride;

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
  auto do_upload = [&] (ch_ptr<VideoPipeline> &pipe, szt &score)
    {
      auto texture = pipe->texture.lock ();
      if (! texture)
        return;

      GstVideoInfo video_info;
      gst_ptr<GstSample> sample = pipe->terminus->FetchClearSample();
      if (sample)
        {
          upload_frame(sample, texture, &video_info);
          score += 3;

          if (pipe->mattes.empty ())
            return;

          GstBuffer *buffer = gst_sample_get_buffer (sample.get ());
          guint64 pts = GST_BUFFER_PTS(buffer);

          guint64 frame_num = guint64 (std::round (pts * GST_VIDEO_INFO_FPS_N(&video_info)
                                                   / GST_VIDEO_INFO_FPS_D(&video_info) / f64(1e9)));
          for (MattePipeline &matte : pipe->mattes)
            matte.awaited = i32 (frame_num);
          //printf ("pts: %f, pf %lu\n", pts/f64(1e9), frame_num);
        }

      // check if available mattes
      if (pipe->active_matte < 0 || pipe->mattes.size () == 0)
        return;

      // fprintf (stderr, "looking for a matte (active %ld)!\n", pipe->active_matte);

      // check if pipeline switched mattes, clear other texture
      // new matte may have different size
      bool force_new_texture = pipe->current_matte != pipe->active_matte;
      if (pipe->active_matte >= i64 (pipe->mattes.size ()))
        {
          pipe->active_matte = pipe->current_matte;
          force_new_texture = false;
        }

      MattePipeline &matte = pipe->mattes[pipe->active_matte];
      bool const needs_upload = force_new_texture || matte.awaited >= 0;

      if (! needs_upload)
        return;

      // fprintf (stderr, "attempting to upload frame\n");
      // if not looking for new frame and not switching, then bail
      if (matte.awaited < 0 && ! force_new_texture)
        return;

      // if looking for new matte but don't have it, then bail
      if (matte.awaited >= 0 && ! matte.worker->PopFrame(matte.awaited, matte.last_popped_frame))
        {
          fprintf (stdout, "no matte for %d which seems fishy\n", matte.awaited);
          return;
        }

      MatteFrameUnique &mf = matte.last_popped_frame;
      if (! mf.IsValid ())
        return;

      matte.awaited = -1;
      pipe->current_matte = pipe->active_matte;
      texture->SetMatteDimensions(matte.roi_min, matte.roi_max);

      if (force_new_texture)
        texture->SetNthTexture(3, BGFX_INVALID_HANDLE);

      u8 *data = mf.data.release();
      bgfx::Memory const *mem = bgfx::makeRef(data, mf.data_size, BGFXfree);
      update_or_create_texture(texture->GetNthTexture(3), mf.width, mf.height,
                               1, mf.width, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
      score += 1;
    };

  auto pred_null_texture
    = [] (ch_ptr<VideoPipeline> const &pipe){ return pipe->texture.expired(); };

  RemoveIfErase(m_pipelines, pred_null_texture);

  szt const max_score = 10000u;
  szt score = 0u;
  szt const count = m_pipelines.size ();
  m_upload_position = m_upload_position >= count ? 0 : m_upload_position;
  for (szt i = 0; i < count && score < max_score; ++i)
    {
      do_upload(m_pipelines[m_upload_position], score);
      m_upload_position = (m_upload_position + 1) % count;
    }

  // fprintf (stderr, "pos: %zu, score: %zu\n", m_upload_position, score);
}

void VideoSystem::PollMessages()
{
  for (ch_ptr<VideoPipeline> &pipe : m_pipelines)
    pipe->pipeline->PollMessages();
}

ch_ptr<DecodePipeline> VideoSystem::CreateDecodePipeline (std::string_view _uri)
{
  auto *video_term = new TampVideoTerminus;
  auto *audio_term = new NullTerminus ("audio/x-raw");
  ch_ptr<DecodePipeline> dec {new DecodePipeline};
  if (! dec->OpenVideoFile (_uri, video_term, audio_term))
    return {};

  return dec;
}

ch_ptr<VideoTexture> VideoSystem::CreateVideoTexture (VideoFormat _format)
{
  ch_ptr<VideoTexture>
    txt{new VideoTexture (_format, m_vgr.matte_program,
                          m_vgr.uniforms, ArraySize (m_vgr.uniforms))};

  return txt;
}

MattePipeline VideoSystem::CreateMattePipeline (f64 _loop_start_ts, f64 _loop_end_ts,
                                                i32 _frame_count, fs::path const &_matte_dir,
                                                v2i32 _min, v2i32 _max)
{
  MattePipeline matte;
  matte.worker = make_ch<MatteLoaderWorker> (_loop_start_ts, _frame_count, _matte_dir);
  matte.worker->SetMatteLoaderPool(m_matte_pool.get ());
  matte.start_timestamp = _loop_start_ts;
  matte.end_timestamp = _loop_end_ts;
  matte.roi_min = _min;
  matte.roi_max = _max;
  matte.frame_count = _frame_count;
  matte.dir_path = _matte_dir;

  return matte;
}


VideoBrace VideoSystem::OpenVideoFile (std::string_view _path)
{
  ch_ptr<VideoPipeline> pipeline = make_ch<VideoPipeline> ();

  ch_ptr<VideoTexture> text = pipeline->OpenFile(_path);
  if (! text)
    return {};

  m_pipelines.push_back(pipeline);
  return {pipeline, text};
}

VideoBrace VideoSystem::OpenMatte (std::string_view _uri,
                                   f64 _loop_start_ts, f64 _loop_end_ts,
                                   i32 _frame_count, fs::path const &_matte_dir,
                                   v2i32 _min, v2i32 _max)
{
  ch_ptr<VideoPipeline> pipeline = make_ch<VideoPipeline> ();

  ch_ptr<VideoTexture> text = pipeline->OpenFile(_uri);
  if (! text)
    return {};

  pipeline->AddMatte(_loop_start_ts, _loop_end_ts,
                     _frame_count, _matte_dir, _min, _max);
  pipeline->GetDecoder()->Loop(_loop_start_ts, _loop_end_ts);

  m_pipelines.push_back(pipeline);

  return {pipeline, text};
}

VideoBrace VideoSystem::DuplicateVideo (ch_ptr<VideoPipeline> const &_pipeline)
{
  ch_ptr<VideoPipeline> pipeline = make_ch<VideoPipeline> ();

  ch_ptr<VideoTexture> text = pipeline->OpenFile(_pipeline->path);
  if (! text)
    return {};

  m_pipelines.push_back (pipeline);

  return {pipeline, text};
}

VideoBrace VideoSystem::DuplicateMatte (ch_ptr<VideoPipeline> const &_pipeline,
                                        i64 _matte_index)
{
  if (_matte_index >= i64 (_pipeline->mattes.size ()) ||
      _matte_index < 0)
    return {};

  ch_ptr<VideoPipeline> pipeline = make_ch<VideoPipeline> ();

  ch_ptr<VideoTexture> text = pipeline->OpenFile(_pipeline->path);
  if (! text)
    return {};

  MattePipeline const &matte = _pipeline->mattes[_matte_index];

  pipeline->AddMatte(matte.start_timestamp, matte.end_timestamp,
                     matte.frame_count, matte.dir_path,
                     matte.roi_min, matte.roi_max);

  pipeline->GetDecoder()->Loop(matte.start_timestamp, matte.end_timestamp);

  m_pipelines.push_back(pipeline);

  return {pipeline, text};
}

ch_ptr<DecodePipeline>
VideoSystem::FindDecodePipeline (ch_ptr<VideoTexture> const &_texture)
{
  for (auto &pipe : m_pipelines)
    {
      if (_texture == pipe->texture)
        return pipe->pipeline;
    }

  return {};
}

ch_ptr<VideoPipeline>
VideoSystem::FindVideoPipeline (ch_ptr<VideoTexture> const &_texture)
{
  for (auto &pipe : m_pipelines)
    {
      if (_texture == pipe->texture)
        return pipe;
    }

  return {};
}


//TODO: change this to not mutate vector of pipelines
void VideoSystem::DestroyVideo (VideoTexture *_texture)
{
  auto pred = [_texture] (ch_ptr<VideoPipeline> const &pipe)
  { return _texture == pipe->texture.get_unchecked(); };

  FindIfErase (m_pipelines, pred);
}



bgfx::TextureHandle VideoSystem::GetBlackTexture () const
{
  return m_vgr.black_texture;
}

bgfx::TextureHandle VideoSystem::GetWhiteTexture () const
{
  return m_vgr.white_texture;
}

u64 VideoSystem::GetVideoBGFXState () const
{
  return m_vgr.basic_state;
}

u64 VideoSystem::GetMatteBGFXState () const
{
  return m_vgr.matte_state;
}

VideoSystem::GraphicsResources &VideoSystem::GetGraphicsResources ()
{
  return m_vgr;
}


}
