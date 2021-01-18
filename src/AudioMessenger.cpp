
#include <AudioMessenger.hpp>
#include <MultiSprinkler.hpp>

#include <nlohmann/json.hpp>

#include <lo/lo_cpp.h>

#include <assert.h>
#include <iostream>

#include "tamparams.h"


bool json_is_not_insanely_wrapped = false;


namespace charm  {


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
  SendUnmute ();
  SendFadeIn (); // just in case the audioserver was left in a faded out state
  SendStatus ("Tample booting");
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


void AudioMessenger::SendPlaySound (std::string_view _file, i64 perf_id, i64 coll_id)
{
  assert (m_audio_address);

  nl::json j;
  j["filename"] = _file;
  j["performance_id"] = perf_id;
  j["collage_id"] = coll_id;
  SendMessage ("/ta/play_sound", j.dump ());
}

void AudioMessenger::SendStopSound (i64 perf_id)
{
  assert (m_audio_address);

  nl::json j;
  j["performance_id"] = perf_id;
  SendMessage ("/ta/stop_performance", j.dump ());
}

void AudioMessenger::SendGetSuggestions (stringy_list &extant_atoms,
                                         const std::string &new_atom,
                                         f64 duration, u64 disc_id)
{
  assert (m_audio_address);

  nlohmann::json j;
  j["extant_atoms"] = extant_atoms;
  j["new_atom"] = new_atom;
  j["duration"] = duration;
  j["discussion_id"] = disc_id;
  SendMessage("/ta/get_suggestions", j.dump ());
}

void AudioMessenger::SendStoreSuggestion (const std::string &atom_name,
                                         const std::string &suggested_sound_name)
{
  assert (m_audio_address);

  nlohmann::json j;
  j["atom"] = new_atom;
  j["suggestion"] = duration;
  SendMessage("/ta/store_suggestion", j.dump ());
}

void AudioMessenger::SendStatus(std::string_view _status_string)
{
  assert (m_audio_address);
  nl::json j;
  j["status"] = _status_string;
  SendMessage ("/ta/tample_status", j.dump ());
}

void AudioMessenger::SendFadeIn(f64 fade_time)
{
  assert (m_audio_address);
  nl::json j;
  j["fade_time"] = fade_time;
  SendMessage ("/ta/fade_in", j.dump ());
}

void AudioMessenger::SendFadeOut(f64 fade_time)
{
  assert (m_audio_address);
  nl::json j;
  j["fade_time"] = fade_time;
  SendMessage ("/ta/fade_out", j.dump ());
}

void AudioMessenger::SendCleanSlate()
{
  assert (m_audio_address);
  SendMessage ("/ta/clean_slate");
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
    j = nl::json::parse ((const char *)&(_msg.argv()[0]->s));
  } catch (std::exception &e) {
    fprintf (stderr, "error parsing %s\n", e.what());
    return;
  }

  TASSuggestionEvent evt {_path, std::move (j)};

  fprintf(stderr,"####\n####\n####\nthis, inbound: \n%s\n####\n",
          evt.GetMessage().dump(1).c_str());

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
    m_message (std::move (_message))
{
}

i64 TASMessageEvent::GetMessageID (bool _spit_error) const
{ auto unwrapped = m_message;
  // if (! json_is_not_insanely_wrapped)
  //   unwrapped = m_message . at (0);
  auto it = unwrapped.find ("message_id");
  if (it != unwrapped.end () && it.value ().is_number())
    return it.value ().get<i64> ();

  if (_spit_error)
    fprintf (stderr, "TAS didn't get expected message id for %s\n",
             m_path.c_str ());

  return -1;
}

i64 TASMessageEvent::GetDiscussionID (bool _spit_error) const
{

fprintf(stderr,"$$$$\n$$$$\n$$$$\nthis, inbound: -- %s --\n$$$$\n$$$$\n$$$$\n",
m_message.dump().c_str());

  auto unwrapped = m_message;
  // if (! json_is_not_insanely_wrapped)
  //   unwrapped = m_message . at (0);

fprintf(stderr,"****\n****\n****\nTHEN STRIPPED ONE LEVEL: -- %s --\n****\n****\n****\n",
unwrapped.dump().c_str());

 // auto it = m_message.find ("discussion_id");
 //  if (it != m_message . end ()  &&  it.value () . is_number ())
 //    return it.value () . get <i64> ();
 auto it = unwrapped.find ("discussion_id");
  if (it != unwrapped . end ()  &&  it.value () . is_number ())
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
{ auto unwrapped = m_message;
  // if (! json_is_not_insanely_wrapped)
  //   unwrapped = m_message . at (0);
  if (auto it = unwrapped.find ("suggestions"); it != unwrapped.end ())
    return i64 (it->size ());

  return 0;
}

std::vector<std::string> TASSuggestionEvent::GetSuggestionNames () const
{
  std::vector<std::string> ret;
  ret.reserve (GetSuggestionCount());

  try
    { auto unwrapped = m_message;
      // if (! json_is_not_insanely_wrapped)
      //   unwrapped = m_message . at (0);
      auto fladge = unwrapped . find ("suggestions");
      if (fladge  !=  unwrapped . end ())
        { for (auto &entry  :  *fladge)
            ret . push_back (entry["name"] . get <std::string> ());
        }
      else
        { fprintf (stderr, "more brutal json-v.-liblo-v.-iterator crap here "
                   "in TASSuggestionEvent::GetSuggestionNames() ...\n");
          return {};
        }
    }
  catch (std::exception &e)
    { fprintf (stderr, "couldn't parse suggestion message: %s\n", e.what ());
      return {};
    }

  return ret;
}


}
