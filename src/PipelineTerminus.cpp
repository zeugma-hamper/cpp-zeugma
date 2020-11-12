#include "PipelineTerminus.hpp"

#include <DecodePipeline.hpp>
#include <MatteLoaderPool.hpp>

#include <gst/app/gstappsink.h>

#include <assert.h>

namespace charm {

PipelineTerminus::PipelineTerminus ()
{ }

PipelineTerminus::~PipelineTerminus ()
{ }

f64 PipelineTerminus::CurrentTimestamp () const
{
  return f64 (CurrentTimestampNS()) / 1e9;
}

gint64 PipelineTerminus::CurrentTimestampNS () const
{
  return 0;
}

std::string PipelineTerminus::GetAcceptedCapsString() const
{
  gst_ptr<GstCaps> caps = GetAcceptedCaps ();
  if (! caps)
    return {};

  gchar *caps_str = gst_caps_to_string (caps.get());
  std::string const ret {caps_str};
  g_free (caps_str);
  return ret;
}

BasicPipelineTerminus::BasicPipelineTerminus (std::string_view _accepted_caps)
  : PipelineTerminus(),
    m_sink {nullptr}
{
  m_accepted_caps = gst_ptr {gst_caps_from_string (_accepted_caps.data ())};
}

BasicPipelineTerminus::~BasicPipelineTerminus ()
{
}

gst_ptr<GstCaps> BasicPipelineTerminus::GetAcceptedCaps () const
{
  return m_accepted_caps;
}

bool BasicPipelineTerminus::Accepts (GstCaps *_caps) const
{
  if (_caps && m_accepted_caps && ! m_sink)
    return gst_caps_can_intersect(m_accepted_caps.get (), _caps);

  return false;
}

bool
BasicPipelineTerminus::OnStart (DecodePipeline *_pipeline)
{
  m_pipeline = _pipeline;
  return true;
}

bool
BasicPipelineTerminus::NewDecodedPad (DecodePipeline *,
                                      GstElement *_elem,
                                      GstPad *_src_pad)
{
  gst_ptr<GstCaps> pad_caps {gst_pad_get_current_caps(_src_pad)};

  if (! pad_caps || ! Accepts (pad_caps.get ()))
    return false;

  return HandleDecodedPad(_elem, _src_pad, pad_caps.get ());
}

bool
BasicPipelineTerminus::OnShutdown (DecodePipeline *)
{
  // {
  //   std::unique_lock {m_sample_mutex};
  //   if (m_sample)
  //     m_sample.release();
  // }

  return true;
}

bool BasicPipelineTerminus::HasSink () const
{
  return m_sink;
}

}
