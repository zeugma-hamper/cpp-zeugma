#ifndef GIMME_THE_RAW_BITS_TIMELY
#define GIMME_THE_RAW_BITS_TIMELY

#include <base_types.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <gst/gst.h>
#include <gst/video/video-info.h>

#include <memory>
#include <mutex>
#include <string_view>


namespace charm {

class PipelineTerminus;

struct LoopStatus
{
  gint64 loop_start = -1;
  gint64 loop_end = -1;
  gint64 adjusted_loop_start = -1;
  gint64 adjusted_loop_end = -1;
};

struct QueuedSeek
{
  f64 rate;
  GstFormat format;
  GstSeekFlags flags;
  GstSeekType start_type;
  i64 start;
  GstSeekType stop_type;
  i64 stop;
};

struct DecodePipeline : public CharmBase<DecodePipeline>
{
  friend PipelineTerminus;

  DecodePipeline ();
  ~DecodePipeline ();

  CHARM_DELETE_MOVE_COPY(DecodePipeline);

  void PollMessages ();

  bool OpenVideoFile (std::string_view _uri, PipelineTerminus *_video_terminus,
                      PipelineTerminus *_audio_terminus = nullptr);

  void Play ();
  void Pause ();
  void Seek (f64 _ts);

  void Step (u32 _distance);

  void Loop (f64 _from, f64 _to);

  // timestamp of the latest video buffer
  f64 CurrentTimestamp () const;
  gint64 CurrentTimestampNS () const;

  f64 Duration () const;
  gint64 DurationNanoseconds () const;

  void SetPipelineState (GstState _state);

  bool SeekFull (f64 rate, GstFormat format, GstSeekFlags flags,
                 GstSeekType start_type, i64 start,
                 GstSeekType stop_type, i64 stop);

  void CleanUp ();

  void NewDecodedPad (GstElement *_elem, GstPad *_pad);

  void HandleErrorMessage (GstMessage *);
  void HandleEosMessage (GstMessage *);
  void HandleStateChangedMessage (GstMessage *);
  void HandleSegmentDone (GstMessage *);
  void HandleAsyncDone (GstMessage *);
  void HandleDuration (GstMessage *);

  std::unique_ptr<PipelineTerminus> m_video_terminus;
  std::unique_ptr<PipelineTerminus> m_audio_terminus;
  GstElement *m_pipeline;
  GstElement *m_uridecodebin;
  GstBus *m_bus;

  GstState m_media_state;
  GstState m_pending_state;

  gint64 m_duration;
  f32 m_play_speed;
  bool m_awaiting_async_done;
  bool m_has_eos;
  bool m_has_queued_seek;
  LoopStatus m_loop_status;
  QueuedSeek m_queued_seek;
};

}

#endif //GIMME_THE_RAW_BITS_TIMELY
