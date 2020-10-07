#include "PipelineTerminus.hpp"

#include <gst/app/gstappsink.h>

#include "DecodePipeline.hpp"

namespace charm {

PipelineTerminus::PipelineTerminus ()
{ }

PipelineTerminus::~PipelineTerminus ()
{ }

BasicPipelineTerminus::BasicPipelineTerminus (bool _enable_audio)
  : PipelineTerminus(),
    m_video_sink {nullptr},
    m_audio_sink {nullptr},
    m_sample_mutex {},
    m_sample {nullptr},
    m_enable_audio {_enable_audio}
{ }

BasicPipelineTerminus::~BasicPipelineTerminus ()
{
}

bool
BasicPipelineTerminus::OnStart (DecodePipeline *)
{
  return true;
}

bool
BasicPipelineTerminus::NewDecodedPad (DecodePipeline *_pipe,
                                      GstElement *_elem,
                                      GstPad *_src_pad,
                                      GstCaps *_pad_caps_override)
{
  GstCaps *pad_caps = _pad_caps_override;
  if (! pad_caps)
    pad_caps = gst_pad_get_current_caps(_src_pad);

  if (! pad_caps)
    return false;

  GstStructure *caps_structure = gst_caps_get_structure(pad_caps, 0);
  const char *caps_string = gst_structure_get_name(caps_structure);

  if (g_str_has_prefix (caps_string, "audio/x-raw"))
    HandleAudioPad (_pipe, _elem, _src_pad, caps_string);
  else if (g_str_has_prefix (caps_string, "video/x-raw"))
    HandleVideoPad (_pipe, _elem, _src_pad, caps_string);

  gst_caps_unref (pad_caps);

  return true;
}

bool
BasicPipelineTerminus::OnShutdown (DecodePipeline *)
{
  {
    std::unique_lock {m_sample_mutex};
    if (m_sample)
      m_sample.release();
  }

  return true;
}

void BasicPipelineTerminus::FlushNotify ()
{ }

bool BasicPipelineTerminus::HasSink () const
{
  return m_video_sink || m_audio_sink;
}

gst_ptr<GstSample> BasicPipelineTerminus::FetchSample ()
{
  std::unique_lock lock (m_sample_mutex);
  if (m_sample)
    return m_sample.ref ();

  return {};
}

gst_ptr<GstSample> BasicPipelineTerminus::FetchClearSample ()
{
  std::unique_lock lock (m_sample_mutex);
  if (m_sample)
    {
      gst_ptr<GstSample> ret {std::move (m_sample)};
      m_sample.reset ();
      return ret;
    }

  return {};
}

bool
BasicPipelineTerminus::HandleAudioPad
(DecodePipeline *_pipe, GstElement *, GstPad *_src_pad, const char *)
{
  // TODO: check returns
  if (! m_enable_audio || m_audio_sink != nullptr)
    return true;

  GstElement *playsink = gst_element_factory_make ("playsink", "audio-play-sink");
  if (! playsink)
    return false;

  gst_bin_add(GST_BIN (_pipe->m_pipeline), playsink);
  gst_element_sync_state_with_parent(playsink);

  GstPad *req_pad = gst_element_get_request_pad(playsink, "audio_sink");
  if (! req_pad)
    goto remove_sink;

  if (GST_PAD_LINK_OK != gst_pad_link (_src_pad, req_pad))
    goto remove_pad;

  gst_object_unref (req_pad);
  m_audio_sink = playsink;

  return true;

remove_pad:
  gst_object_ref (req_pad);

remove_sink:
  gst_bin_remove(GST_BIN (_pipe->m_pipeline), playsink);

  return false;
}

static void basic_term_handle_eos (GstAppSink *, gpointer);
static GstFlowReturn basic_term_handle_new_preroll (GstAppSink *_sink, gpointer _pipeline);
static GstFlowReturn basic_term_handle_new_sample (GstAppSink *_sink, gpointer _pipeline);

bool
BasicPipelineTerminus::HandleVideoPad (DecodePipeline *_pipe, GstElement *,
                                       GstPad *_src_pad, const char *)
{
  GstAppSinkCallbacks callbacks {};
  callbacks.eos = basic_term_handle_eos;
  callbacks.new_preroll = basic_term_handle_new_preroll;
  callbacks.new_sample = basic_term_handle_new_sample;

  GstCaps *caps = nullptr;
  GstElement *convert = nullptr;
  GstPad *sink_pad = nullptr;

  GstElement *as = gst_element_factory_make ("appsink", "appsink0");
  if (! as)
    return false;
  gst_bin_add(GST_BIN (_pipe->m_pipeline), as);
  gst_app_sink_set_callbacks(GST_APP_SINK (as), &callbacks, this, nullptr);

  // TODO: expand caps support to save colorspace conversion and PCIe bandwidth
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "NV12", NULL));
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "I420", NULL));
  caps = gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  caps = gst_caps_merge (caps,
                         gst_caps_new_simple ("video/x-raw",
                                              "format", G_TYPE_STRING, "GRAY8", NULL));
  if (! caps)
    goto remove_sink;

  g_object_set (GST_OBJECT (as), "caps", caps, NULL);
  gst_caps_unref (caps);

  convert = gst_element_factory_make ("videoconvert", "vconvert");
  if (! convert)
    goto remove_sink;

  gst_bin_add (GST_BIN(_pipe->m_pipeline), convert);
  if (! gst_element_link(convert, as))
    goto remove_convert;

  sink_pad = gst_element_get_static_pad(convert, "sink");
  if (GST_PAD_LINK_OK != gst_pad_link (_src_pad, sink_pad))
    goto remove_pad;

  gst_object_unref (sink_pad);

  m_video_sink = as;
  gst_element_sync_state_with_parent(convert);
  gst_element_sync_state_with_parent(as);
  fprintf (stderr, "successful video pad\n");
  return true;

remove_pad:
  gst_object_unref (sink_pad);

remove_convert:
  gst_bin_remove (GST_BIN (_pipe->m_pipeline), convert);

remove_sink:
  gst_bin_remove (GST_BIN(_pipe->m_pipeline), as);

  return false;
}

void BasicPipelineTerminus::HandoffSample (GstSample *_sample)
{
  std::unique_lock lock {m_sample_mutex};
  m_sample = gst_ptr<GstSample> {_sample};
}

static void basic_term_handle_eos (GstAppSink *, gpointer)
{
}

static GstFlowReturn basic_term_handle_new_preroll (GstAppSink *_sink, gpointer _terminus)
{
  BasicPipelineTerminus *terminus = (BasicPipelineTerminus *) _terminus;

  GstSample *sample = gst_app_sink_try_pull_preroll (_sink, GST_CLOCK_TIME_NONE);
  if (sample)
    terminus->HandoffSample (sample);

  return GST_FLOW_OK;
}

static GstFlowReturn basic_term_handle_new_sample (GstAppSink *_sink, gpointer _terminus)
{
  BasicPipelineTerminus *terminus = (BasicPipelineTerminus *) _terminus;

  GstSample *sample = gst_app_sink_try_pull_sample (_sink, GST_CLOCK_TIME_NONE);
  if (sample)
    terminus->HandoffSample (sample);
  return GST_FLOW_OK;
}

CachingPipelineTerminus::CachingPipelineTerminus (bool _enable_audio, u32 _max_cached_samples)
  : PipelineTerminus(),
    m_video_sink {nullptr},
    m_audio_sink {nullptr},
    m_sample_mutex {},
    m_max_cached_samples {_max_cached_samples},
    m_samples {},
    m_enable_audio {_enable_audio}
{ }

CachingPipelineTerminus::~CachingPipelineTerminus ()
{
}

bool
CachingPipelineTerminus::OnStart (DecodePipeline *_pipeline)
{
  m_pipeline = _pipeline;
  return true;
}

bool
CachingPipelineTerminus::NewDecodedPad (DecodePipeline *_pipe,
                                      GstElement *_elem,
                                      GstPad *_src_pad,
                                      GstCaps *_pad_caps_override)
{
  GstCaps *pad_caps = _pad_caps_override;
  if (! pad_caps)
    pad_caps = gst_pad_get_current_caps(_src_pad);

  if (! pad_caps)
    return false;

  GstStructure *caps_structure = gst_caps_get_structure(pad_caps, 0);
  const char *caps_string = gst_structure_get_name(caps_structure);

  if (g_str_has_prefix (caps_string, "audio/x-raw"))
    HandleAudioPad (_pipe, _elem, _src_pad, caps_string);
  else if (g_str_has_prefix (caps_string, "video/x-raw"))
    HandleVideoPad (_pipe, _elem, _src_pad, caps_string);

  gst_caps_unref (pad_caps);

  return true;
}

bool
CachingPipelineTerminus::OnShutdown (DecodePipeline *)
{
  {
    std::unique_lock {m_sample_mutex};
    m_samples.clear ();
  }

  return true;
}

void CachingPipelineTerminus::FlushNotify ()
{
  {
    std::unique_lock lock {m_sample_mutex};
    m_samples.clear ();
  }
  m_pipeline->Step (1);
}



bool CachingPipelineTerminus::HasSink () const
{
  return m_video_sink || m_audio_sink;
}

gst_ptr<GstSample> CachingPipelineTerminus::FetchSample ()
{
  std::unique_lock lock (m_sample_mutex);
  if (m_samples.size ())
    return m_samples[0].ref ();

  return {};
}

gst_ptr<GstSample> CachingPipelineTerminus::FetchClearSample ()
{
  bool should_issue_step = false;

  {
    std::unique_lock lock (m_sample_mutex);
    if (m_samples.size () == m_max_cached_samples)
      should_issue_step = true;
    if (m_samples.size ())
      {
        gst_ptr<GstSample> ret {m_samples.front ()};
        m_samples.erase (m_samples.begin ());
        return ret;
      }
  }

  if (should_issue_step)
    m_pipeline->Step (1);

  return {};
}

gst_ptr<GstSample> CachingPipelineTerminus::FetchByOffset (u64 _offset, u64 _mod)
{
  std::unique_lock lock (m_sample_mutex);
  auto end = m_samples.end ();
  for (auto cur = m_samples.begin (); cur != end; ++cur)
    {
      GstBuffer *buffer = gst_sample_get_buffer (cur->get ());
      u64 offset = GST_BUFFER_OFFSET (buffer) % _mod;
      if (offset == _offset)
        return *cur;
    }

  return {};
}

gst_ptr<GstSample> CachingPipelineTerminus::FetchClearByOffset (u64 _offset, u64 _mod)
{
  bool should_issue_step = false;

  {
    std::unique_lock lock (m_sample_mutex);
    if (m_samples.size () == m_max_cached_samples)
      should_issue_step = true;
    auto end = m_samples.end ();
    for (auto cur = m_samples.begin (); cur != end; ++cur)
      {
        GstBuffer *buffer = gst_sample_get_buffer (cur->get ());
        u64 offset = GST_BUFFER_OFFSET (buffer) % _mod;
        if (offset == _offset)
          {
            gst_ptr<GstSample> ret = *cur;
            m_samples.erase (m_samples.begin (), cur+1);
            return ret;
          }
      }
  }

  if (should_issue_step)
    m_pipeline->Step (1);

  return {};
}

gst_ptr<GstSample> CachingPipelineTerminus::FetchNewest ()
{
  std::unique_lock lock {m_sample_mutex};
  if (m_samples.size ())
    return m_samples.back ();

  return {};
}

void CachingPipelineTerminus::ClearCache ()
{
  std::unique_lock lock {m_sample_mutex};
  m_samples.clear ();
}


bool
CachingPipelineTerminus::HandleAudioPad
(DecodePipeline *_pipe, GstElement *, GstPad *_src_pad, const char *)
{
  // TODO: check returns
  if (! m_enable_audio || m_audio_sink != nullptr)
    return true;

  GstElement *playsink = gst_element_factory_make ("playsink", "audio-play-sink");
  if (! playsink)
    return false;

  gst_bin_add(GST_BIN (_pipe->m_pipeline), playsink);
  gst_element_sync_state_with_parent(playsink);

  GstPad *req_pad = gst_element_get_request_pad(playsink, "audio_sink");
  if (! req_pad)
    goto remove_sink;

  if (GST_PAD_LINK_OK != gst_pad_link (_src_pad, req_pad))
    goto remove_pad;

  gst_object_unref (req_pad);
  m_audio_sink = playsink;

  return true;

remove_pad:
  gst_object_ref (req_pad);

remove_sink:
  gst_bin_remove(GST_BIN (_pipe->m_pipeline), playsink);

  return false;
}

static void caching_term_handle_eos (GstAppSink *, gpointer);
static GstFlowReturn caching_term_handle_new_preroll (GstAppSink *_sink, gpointer _pipeline);
static GstFlowReturn caching_term_handle_new_sample (GstAppSink *_sink, gpointer _pipeline);

bool
CachingPipelineTerminus::HandleVideoPad (DecodePipeline *_pipe, GstElement *,
                                       GstPad *_src_pad, const char *)
{
  GstAppSinkCallbacks callbacks {};
  callbacks.eos = caching_term_handle_eos;
  callbacks.new_preroll = caching_term_handle_new_preroll;
  callbacks.new_sample = caching_term_handle_new_sample;

  GstCaps *caps = nullptr;
  GstElement *convert = nullptr;
  GstPad *sink_pad = nullptr;

  GstElement *as = gst_element_factory_make ("appsink", "appsink0");
  if (! as)
    return false;
  gst_bin_add(GST_BIN (_pipe->m_pipeline), as);
  gst_app_sink_set_callbacks(GST_APP_SINK (as), &callbacks, this, nullptr);

  // TODO: expand caps support to save colorspace conversion and PCIe bandwidth
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "NV12", NULL));
  // gst_caps_append (caps, gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "I420", NULL));
  caps = gst_caps_new_simple ("video/x-raw", "format", G_TYPE_STRING, "RGB", NULL);
  caps = gst_caps_merge (caps,
                         gst_caps_new_simple ("video/x-raw",
                                              "format", G_TYPE_STRING, "GRAY8", NULL));
  if (! caps)
    goto remove_sink;

  g_object_set (GST_OBJECT (as), "caps", caps, NULL);
  gst_caps_unref (caps);

  convert = gst_element_factory_make ("videoconvert", "vconvert");
  if (! convert)
    goto remove_sink;

  gst_bin_add (GST_BIN(_pipe->m_pipeline), convert);
  if (! gst_element_link(convert, as))
    goto remove_convert;

  sink_pad = gst_element_get_static_pad(convert, "sink");
  if (GST_PAD_LINK_OK != gst_pad_link (_src_pad, sink_pad))
    goto remove_pad;

  gst_object_unref (sink_pad);

  m_video_sink = as;
  gst_element_sync_state_with_parent(convert);
  gst_element_sync_state_with_parent(as);
  fprintf (stderr, "successful video pad\n");
  return true;

remove_pad:
  gst_object_unref (sink_pad);

remove_convert:
  gst_bin_remove (GST_BIN (_pipe->m_pipeline), convert);

remove_sink:
  gst_bin_remove (GST_BIN(_pipe->m_pipeline), as);

  return false;
}

void CachingPipelineTerminus::HandoffSample (GstSample *_sample)
{
  if (! _sample || ! gst_sample_get_buffer (_sample))
    return;

  bool room_to_grow = false;
  {
    std::unique_lock lock {m_sample_mutex};
    if (m_samples.size () == m_max_cached_samples)
      m_samples.erase(m_samples.begin ());

    m_samples.emplace_back (gst_ptr<GstSample> {_sample});

    if (m_samples.size () < m_max_cached_samples)
      room_to_grow = true;
  }

  if (room_to_grow)
    m_pipeline->Step (1);
}

static void caching_term_handle_eos (GstAppSink *, gpointer)
{
}

static GstFlowReturn caching_term_handle_new_preroll (GstAppSink *_sink, gpointer _terminus)
{
  CachingPipelineTerminus *terminus = (CachingPipelineTerminus *) _terminus;

  GstSample *sample = gst_app_sink_try_pull_preroll (_sink, GST_CLOCK_TIME_NONE);
  if (sample)
    terminus->HandoffSample (sample);

  return GST_FLOW_OK;
}

static GstFlowReturn caching_term_handle_new_sample (GstAppSink *_sink, gpointer _terminus)
{
  CachingPipelineTerminus *terminus = (CachingPipelineTerminus *) _terminus;

  GstSample *sample = gst_app_sink_try_pull_sample (_sink, GST_CLOCK_TIME_NONE);
  if (sample)
    terminus->HandoffSample (sample);
  return GST_FLOW_OK;
}


}
