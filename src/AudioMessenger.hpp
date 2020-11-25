#ifndef TIN_CANS_AND_STRING
#define TIN_CANS_AND_STRING

#include <ZeEvent.h>
#include <ZePublicWaterWorks.hpp>

#include <class_utils.hpp>

#include <nlohmann/json.hpp>

#include <string_view>

namespace lo
{
  class Server;
  class Address;
  class Message;
}

namespace charm
{

namespace nl = nlohmann;

class AudioMessenger
{
 public:
  AudioMessenger ();

  AudioMessenger (std::string_view _host, std::string_view _port);

  ~AudioMessenger ();

  void Connect (std::string_view _host, std::string_view _port);

  void SendMute ();
  void SendUnmute ();

  void SendPlayBoop ();
  // 0-5 are valid boops
  void SendPlayBoop (i32 _index);

  void SendPlaySound (std::string_view _file);

  void SendGetSuggestions ();

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

class ZETASMessageEvent : public ZeEvent
{
 public:
 ZE_EVT_TUMESCE (ZETASMessage, Ze);

  ZETASMessageEvent (std::string_view _path, nl::json &_message);

  i64 GetResponseID () const;

  std::string const &GetPath () const;

  nl::json const &GetMessage () const;


 protected:
  std::string m_path;
  nlohmann::json m_message;
};

}

#endif
