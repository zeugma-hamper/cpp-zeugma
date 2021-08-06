
#include <TampVideoTerminus.hpp>

#include <DecodePipeline.hpp>
#include <MatteLoaderPool.hpp>

#include <vector_utils.hpp>

#include <gst/app/gstappsink.h>

#include <algorithm>

#include <assert.h>


namespace zeugma  {


TampVideoTerminus::TampVideoTerminus ()
  : BasicPipelineTerminus("video/x-raw,format=I420"),
    m_sample_mutex {},
    m_sample_status {SampleStatus::NoSample},
    m_frame_pts {-1},
    m_frame_number {-1},
    m_has_eos {false}
{ }

TampVideoTerminus::~TampVideoTerminus ()
{
}

static void video_term_handle_eos (GstAppSink *, gpointer);
static GstFlowReturn video_term_handle_new_preroll (GstAppSink *_sink, gpointer _pipeline);
static GstFlowReturn video_term_handle_new_sample (GstAppSink *_sink, gpointer _pipeline);

bool TampVideoTerminus::HandleDecodedPad (GstElement *, GstPad *_src_pad, GstCaps *)
{
  fprintf (stderr, "handle decode pad\n");
  GstAppSinkCallbacks callbacks {};
  callbacks.eos = video_term_handle_eos;
  callbacks.new_preroll = video_term_handle_new_preroll;
  callbacks.new_sample = video_term_handle_new_sample;

  GstElement *convert = nullptr;
  GstPad *sink_pad = nullptr;

  GstElement *as = gst_element_factory_make ("appsink", "appsink0");
  if (! as)
    return false;
  gst_app_sink_set_callbacks(GST_APP_SINK (as), &callbacks, this, nullptr);

  // TODO: expand caps support to handle hardware decoding
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "NV12", NULL));
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "I420", NULL));

  gst_app_sink_set_caps(GST_APP_SINK (as), m_accepted_caps.get ());
  gst_app_sink_set_drop (GST_APP_SINK (as), TRUE);
  gst_app_sink_set_max_buffers(GST_APP_SINK (as), 1);
  // gst_base_sink_set_sync(GST_BASE_SINK(as), TRUE);
  // gst_base_sink_set_qos_enabled(GST_BASE_SINK(as), TRUE);
  g_object_set (as, "sync", TRUE, "qos", FALSE, "enable-last-sample", FALSE, NULL);

  convert = gst_element_factory_make ("videoconvert", "vconvert");
  if (! convert)
    goto remove_sink;

  gst_bin_add (GST_BIN(m_pipeline->m_pipeline), convert);
  sink_pad = gst_element_get_static_pad(convert, "sink");
  if (! GST_PAD_LINK_SUCCESSFUL (gst_pad_link (_src_pad, sink_pad)))
    goto remove_pad;

  gst_bin_add(GST_BIN (m_pipeline->m_pipeline), as);
  if (! gst_element_link(convert, as))
    goto remove_convert;

  assert (gst_element_sync_state_with_parent(convert));
  assert (gst_element_sync_state_with_parent(as));

  gst_object_unref (sink_pad);

  m_sink = as;
  fprintf (stderr, "successful video pad\n");
  return true;

remove_convert:
  gst_bin_remove (GST_BIN (m_pipeline->m_pipeline), convert);

remove_pad:
  gst_object_unref (sink_pad);

remove_sink:
  gst_bin_remove (GST_BIN(m_pipeline->m_pipeline), as);

  return false;
}

bool TampVideoTerminus::OnShutdown (DecodePipeline *_dec)
{
  {
    std::unique_lock lock {m_sample_mutex};
    m_buffer_signal.disconnect_all_slots();
    FetchClearSampleInternal();
  }

  return BasicPipelineTerminus::OnShutdown(_dec);
}


void TampVideoTerminus::FlushNotify ()
{
  std::unique_lock {m_sample_mutex};
  m_has_eos = false;
  m_sample_status = SampleStatus::NoSample;
  m_sample.reset ();
}

gint64 TampVideoTerminus::CurrentTimestampNS () const
{
  std::unique_lock {m_sample_mutex};
  return m_frame_pts;
}

i64 TampVideoTerminus::CurrentFrameNumber () const
{
  std::unique_lock {m_sample_mutex};
  return m_frame_number;
}

v2i32 TampVideoTerminus::CurrentFrameRate () const
{
  std::unique_lock {m_sample_mutex};
  if (! m_caps)
    return {0, 1};

  return {m_video_info.fps_n, m_video_info.fps_d};
}

SampleStatus TampVideoTerminus::HasSample ()
{
  std::unique_lock lock (m_sample_mutex);
  return m_sample_status;
}

gst_ptr<GstSample> TampVideoTerminus::FetchSample ()
{
  std::unique_lock lock (m_sample_mutex);
  return FetchSampleInternal();
}

gst_ptr<GstSample> TampVideoTerminus::FetchClearSample ()
{
  std::unique_lock lock (m_sample_mutex);
  return FetchClearSampleInternal();
}

void TampVideoTerminus::NotifyOfEOS ()
{
  std::unique_lock lock {m_sample_mutex};
  m_has_eos = true;
}

void TampVideoTerminus::NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> const &_sample)
{
  // fprintf (stderr, "%p got sample\n", this);
  std::unique_lock lock {m_sample_mutex};
  m_sample_status = _status;
  m_sample = _sample;

  CacheCaps(gst_sample_get_caps (m_sample.get ()));
  UpdateFrameNumber();
  ExerciseBufferCallback();
}

void TampVideoTerminus::NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> &&_sample)
{
  // fprintf (stderr, "%p got sample\n", this);
  std::unique_lock lock {m_sample_mutex};
  m_sample_status = _status;
  m_sample = std::move (_sample);

  CacheCaps(gst_sample_get_caps (m_sample.get ()));
  UpdateFrameNumber();
  ExerciseBufferCallback();
}

SignalConnection TampVideoTerminus::AddBufferCallback (BufferCallback &&_cb)
{
  std::unique_lock lock {m_sample_mutex};
  return m_buffer_signal.connect(std::move (_cb));
}

SignalConnection TampVideoTerminus::AddBufferExCallback (BufferExCallback &&_cb)
{
  std::unique_lock lock {m_sample_mutex};
  return m_buffer_signal.connect_extended(std::move (_cb));
}

void TampVideoTerminus::CacheCaps (GstCaps *_caps)
{
  if (_caps && (! m_caps.get () || ! gst_caps_is_equal(m_caps.get(), _caps)))
    {
      m_caps = gst_ptr<GstCaps> (_caps, ref_tag{});
      assert (gst_video_info_from_caps (&m_video_info, _caps));
    }
}

void TampVideoTerminus::UpdateFrameNumber ()
{
  if (! m_caps || ! m_sample)
    return;

  GstBuffer *buffer = gst_sample_get_buffer (m_sample.get ());
  if (! GST_BUFFER_PTS_IS_VALID(buffer))
    return;

  m_frame_pts = GST_BUFFER_PTS(buffer);
  assert (m_video_info.fps_d != 0);
  m_frame_number
    = (i64) gst_util_uint64_scale_round(1, m_frame_pts * GST_VIDEO_INFO_FPS_N(&m_video_info),
                                        GST_SECOND * GST_VIDEO_INFO_FPS_D(&m_video_info));
}

void TampVideoTerminus::ExerciseBufferCallback ()
{
  GstBuffer *buffer = gst_sample_get_buffer(m_sample.get ());
  m_buffer_signal (buffer, &m_video_info, m_frame_pts, m_frame_number);
}

gst_ptr<GstSample> TampVideoTerminus::FetchSampleInternal ()
{
  return m_sample;
}

gst_ptr<GstSample> TampVideoTerminus::FetchClearSampleInternal ()
{
  gst_ptr<GstSample> ptr = std::exchange (m_sample, {});
  m_sample_status = SampleStatus::NoSample;
  return ptr;
}

static void video_term_handle_eos (GstAppSink *, gpointer _terminus)
{
  TampVideoTerminus *terminus = (TampVideoTerminus *)_terminus;
  terminus->NotifyOfEOS();
}

static GstFlowReturn video_term_handle_new_preroll (GstAppSink *_sink, gpointer _terminus)
{
  TampVideoTerminus *terminus = (TampVideoTerminus *) _terminus;

  GstSample *sample = gst_app_sink_pull_preroll (_sink);
  if (sample)
    terminus->NotifyOfSample (SampleStatus::Preroll, gst_ptr {sample});

  return GST_FLOW_OK;
}

static GstFlowReturn video_term_handle_new_sample (GstAppSink *_sink, gpointer _terminus)
{
  TampVideoTerminus *terminus = (TampVideoTerminus *) _terminus;

  GstSample *sample = gst_app_sink_pull_sample (_sink);

  if (sample)
    terminus->NotifyOfSample (SampleStatus::Play, gst_ptr {sample});

  return GST_FLOW_OK;
}

NullTerminus::NullTerminus (std::string_view _caps)
  : BasicPipelineTerminus(_caps)
{ }


bool NullTerminus::HandleDecodedPad (GstElement *, GstPad *_src_pad, GstCaps *)
{
  assert (m_pipeline && m_pipeline->m_pipeline);

  std::string name {"nsink"};
  gst_ptr<GstPad> sink_pad;
  GstElement *fakesink = gst_element_factory_make ("fakesink", name.c_str ());
  g_object_set (fakesink,
                "enable-last-sample", FALSE,
                "sync", TRUE,
                NULL);
  if (! fakesink)
    goto exit;
  gst_bin_add (GST_BIN (m_pipeline->m_pipeline), fakesink);

  sink_pad = gst_ptr {gst_element_get_static_pad (fakesink, "sink")};
  if (GST_PAD_LINK_SUCCESSFUL (gst_pad_link (_src_pad, sink_pad.get ())))
    {
      assert (gst_element_sync_state_with_parent(fakesink));
      return true;
    }

  gst_bin_remove (GST_BIN (m_pipeline->m_pipeline), fakesink);

exit:
  return false;
}

void NullTerminus::FlushNotify ()
{
}


}
