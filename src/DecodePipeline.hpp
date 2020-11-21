#ifndef GIMME_THE_RAW_BITS_TIMELY
#define GIMME_THE_RAW_BITS_TIMELY

#include <base_types.hpp>
#include <class_utils.hpp>
#include <ch_ptr.hpp>

#include <boost/signals2/signal.hpp>

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

enum class SegmentDoneBehavior
{
  NotLooping,
  Looping
};

struct DecodePipeline;
using SegmentDoneSignal = boost::signals2::signal<void (DecodePipeline *, SegmentDoneBehavior)>;
using SegmentDoneCallback = SegmentDoneSignal::slot_function_type;
using SegmentDoneExCallback = SegmentDoneSignal::extended_slot_function_type;

using EOSSignal = boost::signals2::signal<void (DecodePipeline *)>;
using EOSCallback = EOSSignal::slot_function_type;
using EOSExCallback = EOSSignal::extended_slot_function_type;

struct DecodePipeline : public CharmBase<DecodePipeline>
{
  friend PipelineTerminus;

  enum class MediaStatus
  {
    Void = 0, //shouldn't happen
    Null, //shouldn't happen
    Ready,
    Paused,
    Playing
  };

  DecodePipeline ();
  ~DecodePipeline ();

  CHARM_DELETE_MOVE_COPY(DecodePipeline);

  void PollMessages ();

  bool OpenVideoFile (std::string_view _uri, PipelineTerminus *_video_terminus,
                      PipelineTerminus *_audio_terminus = nullptr);

  void Play ();
  void Pause ();
  void Seek (f64 _ts);

  void SetPlaySpeed (f32 _speed, bool _trick_play = false);
  void TrickModeSeek (f64 _ts, f64 _rate = 10.0);

  MediaStatus GetStatus () const;
  MediaStatus GetPendingStatus () const;
  // true if status doesn't equal pending status
  bool IsInFlux () const;

  void Step (u32 _distance);

  // pass _play_speed as something other than 0.0f to change speed
  // of playback when pipeline is next in PLAY state
  void Loop (f64 _from, f64 _to, f32 _play_speed = 0.0f);
  bool IsLooping () const;

  // timestamp of the latest video buffer
  f64 CurrentTimestamp () const;
  gint64 CurrentTimestampNS () const;

  f64 Duration () const;
  gint64 DurationNanoseconds () const;

  f32 PlaySpeed () const;

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

  boost::signals2::connection AddSegmentDoneCallback (SegmentDoneCallback &&_cb);
  boost::signals2::connection AddSegmentDoneExCallback (SegmentDoneExCallback &&_cb);
  boost::signals2::connection AddEOSCallback (EOSCallback &&_cb);
  boost::signals2::connection AddEOSExCallback (EOSExCallback &&_cb);

  std::unique_ptr<PipelineTerminus> m_video_terminus;
  std::unique_ptr<PipelineTerminus> m_audio_terminus;
  GstElement *m_pipeline;
  GstElement *m_uridecodebin;
  GstBus *m_bus;

  GstState m_media_state;
  GstState m_pending_state;

  gint64 m_duration;
  f32 m_play_speed;
  gint64 m_segment_start;
  gint64 m_segment_end;
  bool m_awaiting_async_done;
  bool m_has_eos;
  bool m_has_queued_seek;
  LoopStatus m_loop_status;
  QueuedSeek m_queued_seek;
  SegmentDoneSignal m_segment_done_signal;
  EOSSignal m_eos_signal;
};

}

#endif //GIMME_THE_RAW_BITS_TIMELY
