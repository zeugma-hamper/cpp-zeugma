#ifndef TAMP_HAS_NEED_YA_KNOW
#define TAMP_HAS_NEED_YA_KNOW

#include <PipelineTerminus.hpp>

#include <boost/signals2/signal.hpp>

namespace charm
{

//buffer, video info, pts, frame number
using BufferSignal = boost::signals2::signal<void (GstBuffer *, GstVideoInfo *, i64, i64)>;
using BufferCallback = BufferSignal::slot_function_type;
using BufferExCallback = BufferSignal::extended_slot_function_type;
using SignalConnection = boost::signals2::connection;

class TampVideoTerminus : public BasicPipelineTerminus
{
 public:
  TampVideoTerminus ();
  ~TampVideoTerminus () override;

  bool HandleDecodedPad (GstElement *_element, GstPad *_pad, GstCaps *_caps) override;
  bool OnShutdown (DecodePipeline *_dec) override;

  void FlushNotify () override;

  gint64 CurrentTimestampNS () const override;

  i64 CurrentFrameNumber () const;

  v2i32 CurrentFrameRate () const;

  SampleStatus HasSample ();
  gst_ptr<GstSample> FetchSample ();
  gst_ptr<GstSample> FetchClearSample ();

  void NotifyOfEOS ();
  void NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> &&_sample);
  void NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> const &_sample);

  SignalConnection AddBufferCallback (BufferCallback &&_cb);
  SignalConnection AddBufferExCallback (BufferExCallback &&_cb);

  //call with m_sample_mutex;
  void CacheCaps (GstCaps *_caps);
  void UpdateFrameNumber ();
  void ExerciseBufferCallback ();
  gst_ptr<GstSample> FetchSampleInternal ();
  gst_ptr<GstSample> FetchClearSampleInternal ();


  mutable std::mutex m_sample_mutex;
  SampleStatus m_sample_status;
  gst_ptr<GstSample> m_sample;
  gst_ptr<GstCaps> m_caps;
  GstVideoInfo m_video_info;
  i64 m_frame_pts;
  i64 m_frame_number;
  bool m_has_eos;
  BufferSignal m_buffer_signal;
};

class NullTerminus : public BasicPipelineTerminus
{
 public:
  NullTerminus (std::string_view _caps);

  bool HandleDecodedPad (GstElement *_element, GstPad *_pad, GstCaps *_caps) override;

  void FlushNotify () override;
};

}
#endif //TAMP_HAS_NEED_YA_KNOW
