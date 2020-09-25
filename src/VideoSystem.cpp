#include <VideoSystem.hpp>

namespace charm
{
static VideoSystem *s_video_system_instance{nullptr};

bool VideoSystem::Initialize ()
{
  s_video_system_instance = new VideoSystem;
  return true;
}

void VideoSystem::ShutDown ()
{
  delete s_video_system_instance;
}

VideoSystem *VideoSystem::GetSystem ()
{
  return s_video_system_instance;
}


VideoSystem::VideoSystem ()
{
  gst_init (nullptr, nullptr);
}

VideoSystem::~VideoSystem ()
{
}


}
