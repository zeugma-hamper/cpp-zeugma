#include "DecodePipeline.hpp"
#include "PipelineTerminus.hpp"

#include <stdio.h>

namespace charm {

DecodePipeline::DecodePipeline ()
  : m_terminus {nullptr},
    m_pipeline {nullptr},
    m_uridecodebin {nullptr},
    m_bus {nullptr},
    m_media_state {GST_STATE_NULL},
    m_pending_state {GST_STATE_NULL},
    m_play_speed {1.0f},
    m_duration {0},
    m_awaiting_async_done {false},
    m_has_eos {false},
    m_has_queued_seek {false}
{ }

DecodePipeline::~DecodePipeline ()
{
  CleanUp ();
}

static void
db_pad_added_handler (GstElement *src, GstPad *pad, DecodePipeline *data);
static gboolean
db_autoplug_continue_handler (GstElement *bin, GstPad *pad,
                              GstCaps *caps, DecodePipeline *data);

void DecodePipeline::PollMessages ()
{
  if (! m_bus)
    return;

  GstMessageType const message_interest
    = (GstMessageType) (GST_MESSAGE_EOS              |
                        GST_MESSAGE_ERROR            |
                        GST_MESSAGE_STATE_CHANGED    |
                        GST_MESSAGE_SEGMENT_DONE     |
                        GST_MESSAGE_DURATION_CHANGED |
                        GST_MESSAGE_ASYNC_DONE);

  GstMessage *message;
  while ((message = gst_bus_pop_filtered(m_bus, message_interest)))
    {
      switch (GST_MESSAGE_TYPE (message)) {
      case GST_MESSAGE_EOS:
        HandleEosMessage (message);
        break;
      case GST_MESSAGE_ERROR:
        HandleErrorMessage (message);
        break;
      case GST_MESSAGE_STATE_CHANGED:
        HandleStateChangedMessage (message);
        break;
      case GST_MESSAGE_SEGMENT_DONE:
        HandleSegmentDone (message);
        break;
      case GST_MESSAGE_DURATION_CHANGED:
        HandleDuration (message);
        break;
      case GST_MESSAGE_ASYNC_DONE:
        HandleAsyncDone (message);
        break;
      default:
        fprintf (stderr, "unexpected message %d\n", GST_MESSAGE_TYPE (message));
        break;
      }

      gst_message_unref (message);
    }
}

bool DecodePipeline::OpenVideoFile (std::string_view _uri, PipelineTerminus *_term)
{
  if (_uri.empty() || ! _term)
    return false;

  m_terminus = std::unique_ptr<PipelineTerminus> {_term};
  gst_init (NULL, NULL);

  m_pipeline = gst_pipeline_new ("decode-pipeline");
  m_uridecodebin = gst_element_factory_make ("uridecodebin", "uridb");

  if (! m_pipeline || ! m_uridecodebin)
    {
      fprintf (stderr, "decode pipeline: couldn't create pipeline or decodebin\n");
      return false;
    }

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
  gst_bin_add (GST_BIN (m_pipeline), m_uridecodebin);
  gst_element_sync_state_with_parent(m_uridecodebin);

  g_object_set (m_uridecodebin, "uri", _uri.data (), NULL);

  g_signal_connect (m_uridecodebin, "pad-added",
                    G_CALLBACK (db_pad_added_handler), this);
  g_signal_connect (m_uridecodebin, "autoplug-continue",
                    G_CALLBACK (db_autoplug_continue_handler), this);

  bool ret = m_terminus->OnStart (this);
  SetState(GST_STATE_PAUSED);

  GstState current, pending;
  GstStateChangeReturn scret = gst_element_get_state (m_pipeline, &current, &pending,
                                                      GST_CLOCK_TIME_NONE);
  if (scret != GST_STATE_CHANGE_SUCCESS)
    fprintf (stderr, "pipeline couldn't get to PAUSED; returned %d\n", scret);

  return ret;
}

bool DecodePipeline::OpenMatteSequence (std::string_view _pattern, PipelineTerminus *_term)
{
  if (_pattern.empty() || ! _term)
    return false;

  m_terminus = std::unique_ptr<PipelineTerminus> {_term};
  gst_init (NULL, NULL);

  m_pipeline = gst_pipeline_new("matte-pipeline");

  GstElement *src = gst_element_factory_make("multifilesrc", "mf-src");
  GstCaps *caps = gst_caps_from_string("image/tiff,framerate=(fraction)24/1");
  g_object_set (G_OBJECT (src), "caps", caps, "index", 0,
                "location", _pattern.data(),
                "loop", TRUE, NULL);

  //TODO: misleading naming
  m_uridecodebin = gst_element_factory_make ("avdec_tiff", "dec-tiff");
  gst_bin_add_many(GST_BIN (m_pipeline), src, m_uridecodebin, NULL);
  gst_element_sync_state_with_parent(src);
  gst_element_sync_state_with_parent(m_uridecodebin);

  gst_element_link(src, m_uridecodebin);
  GstElement *rate = gst_element_factory_make("videorate", "rater");
  gst_bin_add (GST_BIN (m_pipeline), rate);
  gst_element_sync_state_with_parent(rate);
  gst_element_link(m_uridecodebin, rate);


  GstPad *src_pad = gst_element_get_static_pad(rate, "src");
  GstCaps *template_caps = gst_pad_get_pad_template_caps(src_pad);
  bool ret = m_terminus->OnStart (this);
  m_terminus->NewDecodedPad(this, m_uridecodebin, src_pad, template_caps);

  // g_signal_connect (m_uridecodebin, "pad-added",
  //                   G_CALLBACK (db_pad_added_handler), this);
  // g_signal_connect (m_uridecodebin, "autoplug-continue",
  //                   G_CALLBACK (db_autoplug_continue_handler), this);


  SetState(GST_STATE_PAUSED);
  return ret;
}

void DecodePipeline::Play ()
{
  SetState (GST_STATE_PLAYING);
}

void DecodePipeline::Seek (double _ts)
{
  if (! m_pipeline)
    return;

  gint64 const seconds_to_ns = 1000000000;

  gst_element_seek (m_pipeline, 1.0,
                    GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                    GST_SEEK_TYPE_SET, (gint64)(_ts * seconds_to_ns),
                    GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}


void DecodePipeline::Pause ()
{
  SetState (GST_STATE_PAUSED);
}

// static void loop_seek (GstElement *_elem, gint64 _start_ts, gint64 _end_ts, bool _flush)
// {
//   GstSeekFlags const seek_flags = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT |
//                                                  GST_SEEK_FLAG_ACCURATE);
//                                                  //GST_SEEK_FLAG_KEY_UNIT  |
//                                                  //GST_SEEK_FLAG_TRICKMODE |
//                                                  //GST_SEEK_FLAG_TRICKMODE_NO_AUDIO);
//   GstSeekFlags const flush_seek_flags
//     = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | seek_flags);

//   gst_element_seek (_elem, 1.0,
//                     GST_FORMAT_TIME,
//                     _flush ? flush_seek_flags : seek_flags,
//                     GST_SEEK_TYPE_SET, _start_ts,
//                     GST_SEEK_TYPE_SET, _end_ts);
// }
void DecodePipeline::Step (u32 _distance)
{
  GstEvent *event = gst_event_new_step (GST_FORMAT_BUFFERS, _distance, 1.0, TRUE, FALSE);
  gst_element_send_event(m_pipeline, event);
}


void DecodePipeline::Loop (f64 _from, f64 _to)
{
  if (! m_pipeline)
    return;

  gint64 const seconds_to_ns = 1000000000;

  gint64 const from_ns = gint64(_from * seconds_to_ns);
  gint64 const to_ns = gint64(_to * seconds_to_ns);

  GstSeekFlags const seek_flags = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT |
                                                 GST_SEEK_FLAG_ACCURATE |
                                                 GST_SEEK_FLAG_FLUSH);

  SeekFull(0.0f, GST_FORMAT_TIME, seek_flags,
           GST_SEEK_TYPE_SET, from_ns,
           GST_SEEK_TYPE_SET, to_ns);

  m_loop_status.loop_start = from_ns;
  m_loop_status.loop_end = to_ns;
}

void DecodePipeline::SetState (GstState _state)
{
  if (! m_pipeline)
    return;

  if (_state == m_media_state)
    return;

  GstStateChangeReturn ret = gst_element_set_state (m_pipeline, _state);
  m_pending_state = _state;

  if (ret == GST_STATE_CHANGE_ASYNC)
    m_awaiting_async_done = true;
}

f64 DecodePipeline::Duration () const
{
  return f64 (m_duration) / 1e9;
}

gint64 DecodePipeline::DurationNanoseconds () const
{
  return m_duration;
}

bool DecodePipeline::SeekFull (f64 _rate, GstFormat _format, GstSeekFlags _flags,
                               GstSeekType _start_type, i64 _start,
                               GstSeekType _stop_type, i64 _stop)
{
  if (m_media_state <= GST_STATE_READY || ! m_terminus->HasSink())
    {
      if (m_pending_state <= GST_STATE_READY)
        return false;

      m_queued_seek = {_rate, _format, _flags,
        _start_type, _start, _stop_type, _stop};
      m_has_queued_seek = true;
      return false;
    }

  if (_rate != 0.0)
    m_play_speed = _rate;

  if (_flags & GST_SEEK_FLAG_FLUSH)
    {
      m_awaiting_async_done = true;
      if (m_terminus)
        m_terminus->FlushNotify();
    }

  bool ret = gst_element_seek (m_pipeline, m_play_speed, _format, _flags,
                               _start_type, _start, _stop_type, _stop);

  if (ret)
    m_has_queued_seek = false;

  return ret;
}

void DecodePipeline::CleanUp ()
{
  if (m_terminus)
    {
      m_terminus->OnShutdown (this);
      m_terminus.reset();
    }

  if (m_bus)
    gst_object_unref (m_bus);
  m_bus = nullptr;

  if (m_pipeline)
    {
      gst_element_set_state (m_pipeline, GST_STATE_NULL);
      gst_object_unref (m_pipeline);
    }

  // not holding onto ref to m_uridecodebin
  m_uridecodebin = nullptr;
  m_pipeline = nullptr;
}

void DecodePipeline::HandleErrorMessage (GstMessage *message)
{
  GError *error = nullptr;
  gchar *dbg_info = nullptr;
  gst_message_parse_error(message, &error, &dbg_info);

  fprintf (stderr,"ERROR from element %s: %s\n",
           GST_OBJECT_NAME (message->src), error->message);
  fprintf (stderr, "Debugging info: %s\n", (dbg_info) ? dbg_info : "none");

  g_error_free (error);
  g_free (dbg_info);
}

void DecodePipeline::HandleEosMessage (GstMessage *)
{
  m_has_eos = true;
}

void DecodePipeline::HandleStateChangedMessage (GstMessage *message)
{
  GstState old_state, new_state, pend_state;
  gst_message_parse_state_changed(message, &old_state, &new_state, &pend_state);

  m_media_state = new_state;

  if (m_has_queued_seek)
    {
      SeekFull(m_queued_seek.rate, m_queued_seek.format, m_queued_seek.flags,
               m_queued_seek.start_type, m_queued_seek.start,
               m_queued_seek.stop_type, m_queued_seek.stop);

    }
}

void DecodePipeline::HandleSegmentDone (GstMessage *)
{
  //looping got cancelled
  if (m_loop_status.loop_start < 0 ||
      m_loop_status.loop_end < 0)
    return;

  //continue looping
  GstSeekFlags const seek_flags
    = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_ACCURATE);

  SeekFull (0.0f, GST_FORMAT_TIME, seek_flags,
            GST_SEEK_TYPE_SET, m_loop_status.loop_start,
            GST_SEEK_TYPE_SET, m_loop_status.loop_end);
}

void DecodePipeline::HandleDuration (GstMessage *)
{
  GstQuery *query = nullptr;
  gboolean ret = false;

  query = gst_query_new_duration (GST_FORMAT_TIME);
  ret = gst_element_query (m_pipeline, query);
  if (ret)
    {
      GstFormat format = GST_FORMAT_TIME;
      gint64 duration = 0;
      gst_query_parse_duration(query, &format, &duration);
      m_duration = duration;
    }

  gst_query_unref(query);
}

void DecodePipeline::HandleAsyncDone (GstMessage *)
{
  m_awaiting_async_done = false;
}


/* This function will be called by the pad-added signal */
static void db_pad_added_handler (GstElement *src, GstPad *new_pad, DecodePipeline *data)
{
  data->m_terminus->NewDecodedPad (data, src, new_pad, nullptr);
}

static gboolean db_autoplug_continue_handler (GstElement *, GstPad *,
                                              GstCaps *, DecodePipeline *)
{
  // TODO: here later select the hardware decoder
  // GstStructure *structure = gst_caps_get_structure (caps, 0);
  // gchar const *struct_name = gst_structure_get_name (structure);

  return TRUE;
}

}
