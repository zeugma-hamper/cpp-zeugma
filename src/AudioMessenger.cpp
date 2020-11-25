#include <AudioMessenger.hpp>

#include <lo/lo_cpp.h>

#include <assert.h>

namespace charm
{

AudioMessenger::AudioMessenger ()
  : m_audio_address {nullptr}
{
}

AudioMessenger::AudioMessenger (std::string_view _host, std::string_view _port)
  : m_audio_address {nullptr}
{
  Connect (_host, _port);
}

AudioMessenger::~AudioMessenger ()
{
  delete m_audio_address;
  m_audio_address = nullptr;
}

void AudioMessenger::Connect (std::string_view _host, std::string_view _port)
{
  if (m_audio_address)
    delete m_audio_address;

  m_audio_address = new lo::Address (_host.data (), _port.data ());
  SendUnmute();
}

void AudioMessenger::SendMute ()
{
  assert (m_audio_address);

  m_audio_address->send ("/ta/mute");
}

void AudioMessenger::SendUnmute ()
{
  assert (m_audio_address);

  m_audio_address->send ("/ta/unmute");
}


void AudioMessenger::SendPlayBoop ()
{
  m_audio_address->send ("/ta/play_boop");
}

// 0-5 are valid boops
void AudioMessenger::SendPlayBoop (i32 _index)
{
  assert (m_audio_address);
  assert (_index >= 0 && _index < 6);

  lo::Message msg;
  msg.add_int32(_index);
  m_audio_address->send ("/ta/play_boop", msg);
}


void AudioMessenger::SendPlaySound (std::string_view _file)
{
  assert (m_audio_address);

  lo::Message msg;
  msg.add_string(_file.data ());
  m_audio_address->send ("/ta/play_sound", msg);
}



}
