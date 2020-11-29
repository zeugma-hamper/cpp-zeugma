#include <AudioMessenger.hpp>
#include <MultiSprinkler.hpp>

#include <nlohmann/json.hpp>

#include <lo/lo_cpp.h>

#include <assert.h>
#include <iostream>

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

void AudioMessenger::SendMessage (std::string_view _path)
{
  assert (m_audio_address);

  m_audio_address->send (_path.data ());
}

void AudioMessenger::SendMessage (std::string_view _path, std::string_view _json_msg)
{
  assert (m_audio_address);

  m_audio_address->send (_path.data (), "s", _json_msg.data ());
}

void AudioMessenger::SendMute ()
{
  SendMessage ("/ta/mute");
}

void AudioMessenger::SendUnmute ()
{
  SendMessage ("/ta/unmute");
}


void AudioMessenger::SendPlayBoop ()
{
  SendMessage("/ta/play_boop");
}

// 1-5 are valid boops; 6 chooses a random boop in the 1-5 range
void AudioMessenger::SendPlayBoop (i32 _index)
{
  assert (_index >= 1 && _index <= 6);

  nl::json j;
  j["boopID"] = _index;
  SendMessage("/ta/play_boop", j.dump ());
}


void AudioMessenger::SendPlaySound (std::string_view _file)
{
  assert (m_audio_address);

  nl::json j;
  j["filename"] = _file;
  SendMessage ("/ta/play_sound", j.dump ());
}

void AudioMessenger::SendGetSuggestions (stringy_list &extant_atoms,
                                         const std::string &new_atom,
                                         u64 disc_id)
{
  assert (m_audio_address);

  nlohmann::json j;
  j["extant_atoms"] = extant_atoms;
  j["new_atom"] = new_atom;
//  j["message_id"] = echo_id;
  j["discussion_id"] = disc_id;
  SendMessage("/ta/get_suggestions", j.dump ());
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

  auto handle_sugg = [this] (const char *path, lo::Message const &_msg)
  { this->HandleSuggestions(path, _msg); };

  m_audio_server->add_method ("/taclient/suggestions", "s", std::move (handle_sugg));

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
  nl::json j;
  try {
    j = nl::json::parse ((const char *)_msg.argv()[0]);
  } catch (std::exception &e) {
    fprintf (stderr, "error parsing %s\n", e.what());
    return;
  }

  TASSuggestionEvent evt {_path, std::move (j)};

  m_sprinkler->Spray(&evt);
}

TASMessageEvent::TASMessageEvent (std::string_view _path, nl::json const &_message)
  : ZeEvent (),
    m_path {_path},
    m_message {_message}
{
}

TASMessageEvent::TASMessageEvent (std::string_view _path, nl::json &&_message)
  : ZeEvent (),
    m_path {_path},
    m_message {std::move (_message)}
{
}

i64 TASMessageEvent::GetMessageID (bool _spit_error) const
{
  auto it = m_message.find ("message_id");
  if (it != m_message.end () && it.value ().is_number())
    return it.value ().get<i64> ();

  if (_spit_error)
    fprintf (stderr, "TAS didn't get expected message id for %s\n",
             m_path.c_str ());

  return -1;
}

i64 TASMessageEvent::GetDiscussionID (bool _spit_error) const
{ auto it = m_message.find ("discussion_id");
  if (it != m_message . end ()  &&  it.value () . is_number ())
    return it.value () . get <i64> ();

  if (_spit_error)
    fprintf (stderr, "TAS didn't get a discussion id for %s\n",
             m_path . c_str ());
  return -1;
}

std::string const &TASMessageEvent::GetPath () const
{
  return m_path;
}

nl::json const &TASMessageEvent::GetMessage () const
{
  return m_message;
}

TASSuggestionEvent::TASSuggestionEvent (std::string_view _path, nl::json const &_message)
  : TASMessageEvent (_path, _message)
{
}

TASSuggestionEvent::TASSuggestionEvent (std::string_view _path, nl::json &&_message)
  : TASMessageEvent (_path, std::move (_message))
{
}

i64 TASSuggestionEvent::GetSuggestionCount () const
{
  if (auto it = m_message.find ("suggestions"); it != m_message.end ())
    return i64 (it->size ());

  return 0;
}

std::vector<std::string> TASSuggestionEvent::GetSuggestionNames () const
{
  std::vector<std::string> ret;
  ret.reserve (GetSuggestionCount());

  try
    { auto fladge = m_message . find ("suggestions");
      for (auto &entry : *fladge)
        ret . push_back (entry["name"] . get <std::string> ());
    }
  catch (std::exception &e)
    { fprintf (stderr, "couldn't parse suggestion message: %s\n", e.what ());
      return {};
    }

  return ret;
}

}
