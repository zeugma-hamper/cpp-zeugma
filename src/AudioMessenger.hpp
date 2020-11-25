#ifndef TIN_CANS_AND_STRING
#define TIN_CANS_AND_STRING

#include <class_utils.hpp>

#include <string_view>

namespace lo
{
  class Server;
  class Address;
}

namespace charm
{

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

 protected:
  lo::Address *m_audio_address;
};

}

#endif
