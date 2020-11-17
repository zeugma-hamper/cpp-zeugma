#ifndef TAMP_HAS_NEED_YA_KNOW
#define TAMP_HAS_NEED_YA_KNOW

#include <PipelineTerminus.hpp>

namespace charm
{

class MatteLoaderWorker;

class TampVideoTerminus : public BasicPipelineTerminus
{
 public:
  TampVideoTerminus ();

  bool HandleDecodedPad (GstElement *_element, GstPad *_pad, GstCaps *_caps) override;
  bool OnShutdown (DecodePipeline *_dec) override;

  void FlushNotify () override;

  gint64 CurrentTimestampNS () const override;

  i64 CurrentFrameNumber () const;

  SampleStatus HasSample ();
  gst_ptr<GstSample> FetchSample ();
  gst_ptr<GstSample> FetchClearSample ();

  void NotifyOfEOS ();
  void NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> &&_sample);
  void NotifyOfSample (SampleStatus _status, gst_ptr<GstSample> const &_sample);

  void AddMatteWorker (ch_ptr<MatteLoaderWorker> const &_worker);
  void RemoveMatteLoaderWorker (ch_ptr<MatteLoaderWorker> const &_worker);


  //call with sample_mutex;
  void CacheCaps (GstCaps *_caps);
  void UpdateFrameNumber ();
  void UpdateWorkers ();
  void RemoveWorkerCruft ();

  mutable std::mutex m_sample_mutex;
  SampleStatus m_sample_status;
  gst_ptr<GstSample> m_sample;
  gst_ptr<GstCaps> m_caps;
  GstVideoInfo m_video_info;
  i64 m_frame_pts;
  i64 m_frame_number;
  bool m_has_eos;

  std::vector<ch_weak_ptr<MatteLoaderWorker>> m_matte_workers;
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
