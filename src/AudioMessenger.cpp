#include <AudioMessenger.hpp>
#include <MultiSprinkler.hpp>

#include <nlohmann/json.hpp>

#include <lo/lo_cpp.h>

#include <assert.h>

namespace charm
{

AudioMessenger::AudioMessenger ()
  : m_audio_address {nullptr},
    m_message_id {0}
{
}

AudioMessenger::AudioMessenger (std::string_view _host, std::string_view _port)
  : m_audio_address {nullptr},
    m_message_id {0}
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

  m_audio_address->send ("/ta/play_boop", lo::Message ("i", _index));
}


void AudioMessenger::SendPlaySound (std::string_view _file)
{
  assert (m_audio_address);

  m_audio_address->send ("/ta/play_sound",
                         lo::Message ("s", _file.data ()));
}

void AudioMessenger::SendGetSuggestions ()
{
  assert (m_audio_address);

  nlohmann::json j;
  j["foo"] = "bar";
  m_audio_address->send ("/ta/get_suggestions",
                         lo::Message ("s", j.dump ().c_str ()));
}

//TASReceiver
TASReceiver::TASReceiver ()
  : ZePublicWaterWorks(),
    m_audio_server {nullptr},
    m_sprinkler {nullptr}
{
}


TASReceiver::TASReceiver (std::string_view _port)
  : ZePublicWaterWorks(),
    m_audio_server {nullptr},
    m_sprinkler {nullptr}
{
  Connect (_port);
}


TASReceiver::~TASReceiver ()
{
  Disconnect();
}



void TASReceiver::Connect (std::string_view _port)
{
  m_audio_server = new lo::Server (_port.data (),
                                   [this] (i32 _num, const char *_msg, const char *_where)
                                   { this->LoErrorHandler(_num, _msg, _where); });
  m_audio_server->add_method ("/taclient/suggestions",
                              
}

void TASReceiver::Disconnect ()
{
  delete m_audio_server;
  m_audio_server = nullptr;
}

bool TASReceiver::IsValid ()
{
  return m_audio_server && m_audio_server->is_valid();
}

void TASReceiver::Drain (MultiSprinkler *_sprinkler)
{
  m_sprinkler = _sprinkler;

  if (IsValid ())
    while (m_audio_server->recv (0)  >  0)
      { }
}

i32 TASReceiver::LoErrorHandler (i32 _num, const char *_msg, const char *_where)
{
  fprintf (stderr, "TAS error handler:\n%d - %s\n%s\n",
           _num, _msg, _where);

  return 0;
}

void TASReceiver::HandleSuggestions (const char *_path, lo::Message const &_msg)
{
  nl::json j = json::parse ((const char *)_msg.argv()[0]);
  
}

ZETASMessageEvent::ZETASMessageEvent (std::string_view _path, nl::json &_message)
  : ZeEvent (),
    m_path {_path},
    m_message {_message}
{
}

i64 ZETASMessageEvent::GetResponseID () const
{
  return -1;
}

std::string const &ZETASMessageEvent::GetPath () const
{
  return m_path;
}

nl::json const &ZETASMessageEvent::GetMessage () const
{
  return m_message;
}


}
