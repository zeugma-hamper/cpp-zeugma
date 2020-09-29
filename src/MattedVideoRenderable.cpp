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
  : Renderable ()
{
  m_video_texture = VideoSystem::GetSystem ()
    ->OpenMatte (_uri, _loop_start_ts, _loop_end_ts, _matte_dir);
}

MattedVideoRenderable::~MattedVideoRenderable ()
{
}

// void MattedVideoRenderable::UploadSample (gst_ptr<GstSample> const &_sample)
// {
//   GstCaps *sample_caps = gst_sample_get_caps(_sample.get ());
//   GstVideoInfo video_info;
//   int width, height, components, stride;

//   if (! sample_caps)
//     return;

//   gst_video_info_init(&video_info);
//   if (! gst_video_info_from_caps (&video_info, sample_caps))
//     return;

//   if (! GST_VIDEO_INFO_IS_RGB (&video_info))
//     return;

//   width = GST_VIDEO_INFO_WIDTH (&video_info);
//   height = GST_VIDEO_INFO_HEIGHT (&video_info);
//   components = GST_VIDEO_INFO_N_COMPONENTS (&video_info);
//   //RGB is all one plane
//   stride = GST_VIDEO_INFO_PLANE_STRIDE(&video_info, 0);
//   // align = calculate_alignment (stride);
//   (void)components;

//   m_aspect_ratio = width / (f32)height;

//   if (! bgfx::isValid(m_texture))
//     {
//       fprintf (stderr, "create video texture\n");
//       bgfx::TextureFormat::Enum const formats[5]
//         { bgfx::TextureFormat::Unknown, //shouldn't happen
//           bgfx::TextureFormat::R8,
//           bgfx::TextureFormat::RG8,
//           bgfx::TextureFormat::RGB8,
//           bgfx::TextureFormat::RGBA8 };

//       m_texture = bgfx::createTexture2D(width, height, false, 1,
//                                         formats[components],
//                                         BGFX_SAMPLER_UVW_CLAMP |
//                                         BGFX_SAMPLER_POINT);
//     }

//   video_frame_holder *frame_holder = new video_frame_holder (_sample, &video_info);

//   const bgfx::Memory *mem = bgfx::makeRef (GST_VIDEO_FRAME_PLANE_DATA(&frame_holder->video_frame, 0),
//                                            GST_VIDEO_FRAME_SIZE(&frame_holder->video_frame),
//                                            DeleteImageLeftOvers<video_frame_holder>, frame_holder);

//   bgfx::updateTexture2D(m_texture, 0, 0, 0, 0, width, height, mem, stride);


//   GstBuffer *buffer = gst_sample_get_buffer (_sample.get ());
//   guint64 pts = GST_BUFFER_PTS(buffer);

//   if (gint64(pts) < m_video_pipeline->m_loop_status.loop_start ||
//       gint64(pts) > m_video_pipeline->m_loop_status.loop_end)
//     return;

//   //i expect looping here for now
//   guint64 offset_ns = pts - m_video_pipeline->m_loop_status.loop_start;
//   guint64 frame_num = guint64 (offset_ns * GST_VIDEO_INFO_FPS_N(&video_info)
//                                / (GST_VIDEO_INFO_FPS_D(&video_info) * f64(1e9)));
//   printf ("pts: %.3f, now: %.3f\n", pts/f64(1e9), offset_ns/f64(1e9));

//   auto &matte = m_mattes[frame_num];
//   assert (frame_num < m_mattes.size ());

//   (void)matte;
//   if (bgfx::isValid(m_matte))
//     bgfx::destroy (m_matte);
//   m_matte = LoadTexture2D (matte.path().c_str(), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);
//   if (! bgfx::isValid(m_matte))
//     fprintf (stderr, "returned handle isn't valid\n");
// }

// void MattedVideoRenderable::Update ()
// {
//   if (m_video_pipeline)
//     {
//       m_video_pipeline->PollMessages ();
//       gst_ptr<GstSample> new_sample = m_terminus->FetchClearSample();

//       if (new_sample)
//         UploadSample(new_sample);
//     }
// }

void MattedVideoRenderable::Draw ()
{
  if (! m_video_texture || ! bgfx::isValid(m_video_texture->GetNthTexture(0)))
    return;

  bgfx::setTransform(&m_node->GetAbsoluteTransformation().model);

  m_video_texture->BindGraphics (BGFX_STATE_PT_TRISTRIP);
  bgfx::setVertexCount(4);

  glm::vec4 unity {1.0f, 1080.0f/1920.0f, 1.0f, 1.0f};
  bgfx::setUniform(m_video_texture->GetAspectUniform(), glm::value_ptr (unity));

  bgfx::submit(0, m_video_texture->GetProgram());
}

}
