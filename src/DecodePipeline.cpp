#include "DecodePipeline.hpp"
#include "PipelineTerminus.hpp"

#include <stdio.h>

#include <assert.h>

namespace charm {

[[maybe_unused]]
static const char *s_state_names[5] {"void pending", "null", "ready", "paused", "playing"};
[[maybe_unused]]
static const char *s_sc_return_names[4] {"failure", "success", "async", "no preroll"};

DecodePipeline::DecodePipeline ()
  : m_pipeline {nullptr},
    m_uridecodebin {nullptr},
    m_bus {nullptr},
    m_media_state {GST_STATE_NULL},
    m_pending_state {GST_STATE_NULL},
    m_duration {0},
    m_play_speed {1.0f},
    m_segment_start {-1},
    m_segment_end {-1},
    m_awaiting_async_done {false},
    m_has_eos {false},
    m_has_queued_seek {false}
{
}

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

bool DecodePipeline::OpenVideoFile (std::string_view _uri, PipelineTerminus *_video_terminus,
                                    PipelineTerminus *_audio_terminus)
{
  if (_uri.empty() || ! _video_terminus)
    return false;

  m_video_terminus = std::unique_ptr<PipelineTerminus> {_video_terminus};
  m_audio_terminus = std::unique_ptr<PipelineTerminus> {_audio_terminus};
  gst_init (NULL, NULL);

  m_pipeline = gst_pipeline_new ("decode-pipeline");

  m_uridecodebin = gst_element_factory_make ("uridecodebin", "db");

  if (! m_pipeline || ! m_uridecodebin)
    {
      fprintf (stderr, "decode pipeline: couldn't create pipeline or decodebin\n");
      return false;
    }

  gst_ptr<GstCaps> vid_caps = m_video_terminus->GetAcceptedCaps();
  gst_ptr<GstCaps> raw_caps = gst_ptr {vid_caps ? gst_caps_copy(vid_caps.get ()) : gst_caps_new_empty()};
  if (m_audio_terminus)
    raw_caps = gst_ptr {gst_caps_merge (raw_caps.transfer (),
                                        gst_caps_copy (m_audio_terminus->GetAcceptedCaps().get ()))};
  else
    raw_caps = gst_ptr {gst_caps_merge (raw_caps.transfer(),
                                        gst_caps_from_string("audio/x-raw"))};

  gchar *cstr = gst_caps_to_string (raw_caps.get ());
  fprintf (stderr, "we accept %s\n", cstr);
  g_free(cstr);

  g_object_set (m_uridecodebin,
                "uri", _uri.data (),
                "caps", raw_caps.get(),
                // "expose-all-streams", FALSE,
                NULL);

  m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
  gst_bin_add (GST_BIN (m_pipeline), m_uridecodebin);
  assert (gst_element_sync_state_with_parent(m_uridecodebin));


  g_signal_connect (m_uridecodebin, "pad-added",
                    G_CALLBACK (db_pad_added_handler), this);
  // g_signal_connect (m_uridecodebin, "autoplug-continue",
  //                   G_CALLBACK (db_autoplug_continue_handler), this);

  bool ret = m_video_terminus->OnStart (this);
  if (m_audio_terminus)
    ret = ret && m_audio_terminus->OnStart (this);
  SetPipelineState(GST_STATE_PAUSED);

  GstState current, pending;
  GstStateChangeReturn scret = gst_element_get_state (m_pipeline, &current, &pending,
                                                      GST_CLOCK_TIME_NONE);
  if (scret != GST_STATE_CHANGE_SUCCESS)
    fprintf (stderr, "pipeline couldn't get to PAUSED; returned %d\n", scret);

  return ret;
}


void DecodePipeline::Play ()
{
  SetPipelineState (GST_STATE_PLAYING);
}

void DecodePipeline::Seek (double _ts)
{
  if (IsLooping())
    {
      gint64 current_ts = 0;
      if (! gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &current_ts))
        {
          fprintf (stderr, "couldn't query position. bailing.\n");
          return;
        }

      GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_ACCURATE);
      if (m_pending_state != GST_STATE_PLAYING)
        flags = GstSeekFlags (flags | GST_SEEK_FLAG_FLUSH);

      SeekFull (m_play_speed,
                GST_FORMAT_TIME,
                flags,
                GST_SEEK_TYPE_SET, 0,
                GST_SEEK_TYPE_END, 0);
    }

  gint64 const seconds_to_ns = 1000000000;

  SeekFull (m_play_speed,
            // GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SNAP_AFTER |
            //                                 GST_SEEK_FLAG_FLUSH),
            GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH |
                                            GST_SEEK_FLAG_ACCURATE),
            GST_SEEK_TYPE_SET, (gint64)(_ts * seconds_to_ns),
            GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
}


void DecodePipeline::Pause ()
{
  SetPipelineState (GST_STATE_PAUSED);
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
void DecodePipeline::SetPlaySpeed (f64 _speed, bool _trick_play)
{
  gint64 ts = 0;
  if (! gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &ts))
    {
      fprintf (stderr, "couldn't query position. bailing.\n");
      return;
    }

  GstSeekFlags flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH |
                                       GST_SEEK_FLAG_ACCURATE);
  if (_trick_play)
    flags = (GstSeekFlags) (flags | GST_SEEK_FLAG_TRICKMODE);

  if (_speed > 0)
    SeekFull(_speed, GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, ts, GST_SEEK_TYPE_NONE, 0);
  else
    SeekFull(_speed, GST_FORMAT_TIME, flags, GST_SEEK_TYPE_SET, ts, GST_SEEK_TYPE_NONE, 0);
}

void DecodePipeline::TrickModeSeek (f64 _ts, f64 _rate)
{
  f64 const rate = _rate < 0.0 ? -_rate : _rate;
  if (rate <= 1.0)
    return;

  gint64 const ts = _ts * 1e9;
  fprintf (stderr, "trick mode seek to %f or %ld\n", _ts, ts);

  gint64 current_ts = 0;
  if (! gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &current_ts))
    {
      fprintf (stderr, "couldn't query position. bailing.\n");
      return;
    }

  GstSeekFlags flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH |
                                       GST_SEEK_FLAG_ACCURATE |
                                       GST_SEEK_FLAG_TRICKMODE |
                                       GST_SEEK_FLAG_SEGMENT);
  // GstSeekFlags flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE |
  //                                      GST_SEEK_FLAG_TRICKMODE |
  //                                      GST_SEEK_FLAG_SEGMENT);

  if (current_ts < ts)
    SeekFull(rate, GST_FORMAT_TIME, flags,
             GST_SEEK_TYPE_SET, current_ts,
             GST_SEEK_TYPE_SET, ts);
  else if (ts < current_ts)
    SeekFull(-rate, GST_FORMAT_TIME, flags,
             GST_SEEK_TYPE_SET, ts,
             GST_SEEK_TYPE_SET, current_ts);
  else
    return;

  if (m_pending_state != GST_STATE_PLAYING)
    Play ();
}

MediaStatus DecodePipeline::GetStatus () const
{
  return MediaStatus (m_media_state);
}

MediaStatus DecodePipeline::GetPendingStatus () const
{
  return MediaStatus (m_pending_state);
}

bool DecodePipeline::IsInFlux () const
{
  return m_media_state != m_pending_state;
}

void DecodePipeline::Step (u32 _distance)
{
  GstEvent *event = gst_event_new_step (GST_FORMAT_BUFFERS, _distance, 1.0, TRUE, FALSE);
  gst_element_send_event(m_pipeline, event);
}


void DecodePipeline::Loop (f64 _from, f64 _to, f32 _play_speed)
{
  if (! m_pipeline)
    return;

  gint64 const seconds_to_ns = 1000000000;

  gint64 const from_ns = gint64(_from * seconds_to_ns);
  gint64 const to_ns = gint64(_to * seconds_to_ns);

  GstSeekFlags const seek_flags = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT |
                                                 GST_SEEK_FLAG_ACCURATE |
                                                 GST_SEEK_FLAG_FLUSH);

  SeekFull(_play_speed, GST_FORMAT_TIME, seek_flags,
           GST_SEEK_TYPE_SET, from_ns,
           GST_SEEK_TYPE_SET, to_ns);

  //seek full with segment will modify loop status, so set to correct
  //values after seek.
  m_loop_status.loop_start = from_ns;
  m_loop_status.loop_end = to_ns;
}

bool DecodePipeline::IsLooping () const
{
  return m_loop_status.loop_start >= 0 &&
    m_loop_status.loop_end >= 0;
}

f64 DecodePipeline::CurrentPosition () const
{
  return f64(CurrentPositionNS ()) / f64 (1e9);
}

gint64 DecodePipeline::CurrentPositionNS () const
{
  gint64 ts = -1;
  if (gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &ts))
    return ts;

  return ts;
}

f64 DecodePipeline::CurrentTimestamp () const
{
  if (m_video_terminus)
    return m_video_terminus->CurrentTimestamp ();

  return 0.0;
}

gint64 DecodePipeline::CurrentTimestampNS () const
{
  if (m_video_terminus)
    return m_video_terminus->CurrentTimestampNS ();

  return 0;
}

v2i32 DecodePipeline::CurrentVideoFrameRateAsRatio () const
{
  if (m_video_terminus)
    return m_video_terminus->CurrentFrameRate ();

  return {0, 1};
}


v2i32 DecodePipeline::CurrentAudioFrameRateAsRatio () const
{
  if (m_audio_terminus)
    return m_audio_terminus->CurrentFrameRate ();

  return {0, 1};
}


f64 DecodePipeline::Duration () const
{
  return f64 (m_duration) / 1e9;
}

gint64 DecodePipeline::DurationNanoseconds () const
{
  return m_duration;
}

f32 DecodePipeline::PlaySpeed () const
{
  return m_play_speed;
}

void DecodePipeline::SetPipelineState (GstState _state)
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

bool DecodePipeline::SeekFull (f64 _rate, GstFormat _format, GstSeekFlags _flags,
                               GstSeekType _start_type, i64 _start,
                               GstSeekType _stop_type, i64 _stop)
{
  if (m_media_state <= GST_STATE_READY || ! m_video_terminus->HasSink())
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
      if (m_video_terminus)
        m_video_terminus->FlushNotify();
      if (m_audio_terminus)
        m_audio_terminus->FlushNotify();
    }

  if (_flags & GST_SEEK_FLAG_SEGMENT)
    {
      m_segment_start = _start;
      m_segment_end = _stop;

      if (IsLooping())
        {
          m_loop_status.loop_start = -1;
          m_loop_status.loop_end = -1;
        }
    }

  GstElement *elem = m_video_terminus->GetSink();
  assert (elem);
  bool ret = gst_element_seek (elem, m_play_speed, _format, _flags,
                               _start_type, _start, _stop_type, _stop);
  // bool ret = gst_element_seek (m_pipeline, m_play_speed, _format, _flags,
  //                              _start_type, _start, _stop_type, _stop);

  if (ret)
    m_has_queued_seek = false;

  return ret;
}

void DecodePipeline::CleanUp ()
{
  m_segment_done_signal.disconnect_all_slots();
  m_eos_signal.disconnect_all_slots();

  if (m_pipeline)
    {
      gst_element_set_state (m_pipeline, GST_STATE_NULL);
      GstState state, pstate;
      gst_element_get_state (m_pipeline, &state, &pstate, GST_CLOCK_TIME_NONE);
    }

  if (m_video_terminus)
    {
      m_video_terminus->OnShutdown (this);
      m_video_terminus.reset();
    }

  if (m_audio_terminus)
    {
      m_audio_terminus->OnShutdown (this);
      m_audio_terminus.reset();
    }

  if (m_bus)
    gst_object_unref (m_bus);
  m_bus = nullptr;

  if (m_pipeline)
    gst_object_unref (m_pipeline);

  // not holding onto ref to m_uridecodebin
  m_uridecodebin = nullptr;
  m_pipeline = nullptr;
}

void DecodePipeline::NewDecodedPad (GstElement *_element, GstPad *_pad)
{
  gst_ptr<GstCaps> caps = gst_ptr {gst_pad_get_current_caps(_pad)};
  if (m_video_terminus->Accepts(caps.get ()))
    m_video_terminus->NewDecodedPad(this, _element, _pad);
  else if (m_audio_terminus && m_audio_terminus->Accepts(caps.get ()))
    m_audio_terminus->NewDecodedPad(this, _element, _pad);
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
  m_eos_signal (this);
  m_has_eos = true;
}

void DecodePipeline::HandleStateChangedMessage (GstMessage *message)
{
  GstState old_state, new_state, pend_state;
  gst_message_parse_state_changed(message, &old_state, &new_state, &pend_state);

  if (m_pipeline != GST_ELEMENT (GST_MESSAGE_SRC (message)))
    return;

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
  // fprintf (stderr, "got segment done!\n");
  //looping got cancelled, clear segment
  if (m_loop_status.loop_start < 0 ||
      m_loop_status.loop_end < 0)
    {
      // gint64 current_ts = 0;
      // if (! gst_element_query_position(m_pipeline, GST_FORMAT_TIME, &current_ts))
      //   {
      //     fprintf (stderr, "couldn't query position. bailing.\n");
      //     return;
      //   }

      // gint64 const seg_ts = m_play_speed > 0.0 ? m_segment_end : m_segment_start;

      // SeekFull (1.0,
      //           GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_ACCURATE),
      //           GST_SEEK_TYPE_SET, 0,
      //           GST_SEEK_TYPE_END, 0);

      // SeekFull (1.0,
      //           GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
      //           GST_SEEK_TYPE_SET, seg_ts,
      //           GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
      // Pause ();

      m_segment_done_signal (this, SegmentDoneBehavior::NotLooping);
      return;
    }



  //continue looping
  GstSeekFlags seek_flags
    = (GstSeekFlags)(GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_ACCURATE);
  if (m_pending_state != GST_STATE_PLAYING)
    seek_flags = (GstSeekFlags)(seek_flags | GST_SEEK_FLAG_FLUSH);

  SeekFull (0.0f, GST_FORMAT_TIME, seek_flags,
            GST_SEEK_TYPE_SET, m_loop_status.loop_start,
            GST_SEEK_TYPE_SET, m_loop_status.loop_end);

  m_segment_done_signal (this, SegmentDoneBehavior::Looping);
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
      if (m_segment_start < 0)
        m_segment_start = 0;
      if (m_segment_end < 0)
        m_segment_end = duration;
    }

  gst_query_unref(query);
}

void DecodePipeline::HandleAsyncDone (GstMessage *)
{
  m_awaiting_async_done = false;
  m_async_done_signal (this);
}

boost::signals2::connection
DecodePipeline::AddSegmentDoneCallback (SegmentDoneCallback &&_cb)
{
  return m_segment_done_signal.connect (std::forward<SegmentDoneCallback> (_cb));
}

boost::signals2::connection
DecodePipeline::AddSegmentDoneExCallback (SegmentDoneExCallback &&_cb)
{
  return m_segment_done_signal.connect_extended (std::forward<SegmentDoneExCallback> (_cb));
}

boost::signals2::connection
DecodePipeline::AddEOSCallback (EOSCallback &&_cb)
{
  return m_eos_signal.connect (std::forward<EOSCallback> (_cb));
}

boost::signals2::connection
DecodePipeline::AddEOSExCallback (EOSExCallback &&_cb)
{
  return m_eos_signal.connect_extended (std::forward<EOSExCallback> (_cb));
}

/* This function will be called by the pad-added signal */
static void db_pad_added_handler (GstElement *src, GstPad *new_pad, DecodePipeline *data)
{
  fprintf (stderr, "pad added\n");
  data->NewDecodedPad (src, new_pad);
}

[[maybe_unused]]
static gboolean db_autoplug_continue_handler (GstElement *, GstPad *,
                                              GstCaps *, DecodePipeline *)
{
  // TODO: here later select the hardware decoder
  // GstStructure *structure = gst_caps_get_structure (caps, 0);
  // gchar const *struct_name = gst_structure_get_name (structure);

  return TRUE;
}

}
