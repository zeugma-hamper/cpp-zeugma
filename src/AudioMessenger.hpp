
#ifndef TIN_CANS_AND_STRING
#define TIN_CANS_AND_STRING


#include <ZeEvent.h>
#include <ZePublicWaterWorks.hpp>

#include "tamparams.h"

#include <class_utils.hpp>

#include <nlohmann/json.hpp>

#include <string_view>


namespace lo
{
  class Server;
  class Address;
  class Message;
}


namespace charm  {

namespace nl = nlohmann;


class AudioMessenger
{
 public:
  AudioMessenger ();

  AudioMessenger (std::string_view _host, std::string_view _port);

  ~AudioMessenger ();

  void Connect (std::string_view _host, std::string_view _port);

  void SendMessage (std::string_view _path);
  void SendMessage (std::string_view _path, std::string_view _json_msg);

  void SendMute ();
  void SendUnmute ();

  void SendPlayBoop ();
  // 0-4 are valid boops
  void SendPlayBoop (i32 _index);

  void SendPlaySound (std::string_view _file);

  void SendGetSuggestions (stringy_list &extant_atoms,
                           const std::string &new_atom, u64 disc_id);

 protected:
  lo::Address *m_audio_address;
  i64 m_message_id;
};


class TASReceiver : public ZePublicWaterWorks
{
 public:
  TASReceiver ();

  TASReceiver (std::string_view _port);

  ~TASReceiver () override;

  void Connect (std::string_view _port);
  void Disconnect ();
  bool IsValid ();

  void Drain (MultiSprinkler *_sprinkler) override;

  i32 LoErrorHandler (i32 _num, const char *_msg, const char *_where);

 protected:

  void HandleSuggestions (const char *_path, lo::Message const &_msg);

  lo::Server *m_audio_server;
  MultiSprinkler *m_sprinkler;
};


class TASMessageEvent : public ZeEvent
{
 public:
 ZE_EVT_TUMESCE (TASMessage, Ze);

  TASMessageEvent (std::string_view _path, nl::json const &_message);
  TASMessageEvent (std::string_view _path, nl::json &&_message);

  i64 GetMessageID (bool _spit_error = false) const;
  i64 GetDiscussionID (bool _spit_error = false)  const;

  std::string const &GetPath () const;

  nl::json const &GetMessage () const;


 protected:
  std::string m_path;
  nlohmann::json m_message;
};


class TASSuggestionEvent : public TASMessageEvent
{
 public:
  ZE_EVT_TUMESCE (TASSuggestion, TASMessage);

  TASSuggestionEvent (std::string_view _path, nl::json const &_message);
  TASSuggestionEvent (std::string_view _path, nl::json &&_message);

  i64 GetSuggestionCount () const;
  std::vector<std::string> GetSuggestionNames () const;
};


}


#endif
