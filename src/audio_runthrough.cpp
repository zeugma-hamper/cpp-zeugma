#include <AudioMessenger.hpp>

#include <string>

#include <unistd.h>

using namespace charm;

static std::string default_host = "127.0.0.1";
static std::string default_port = "57121";


int main (int ac, char **av)
{
  std::string host = default_host;
  std::string port = default_port;

  if (ac > 1)
    host = av[1];

  if (ac > 2)
    port = av[2];

  fprintf (stderr, "address is %s:%s\n", host.c_str (), port.c_str ());
  AudioMessenger messenger (host, port);

  // fprintf (stderr, "play boop\n");
  // messenger.SendPlayBoop();
  sleep (1);

  fprintf (stderr, "muted play boop\n");
  messenger.SendMute();
  sleep (1);
  messenger.SendPlayBoop(0);
  sleep (1);
  messenger.SendUnmute();
  sleep (1);

  for (i32 i = 0; i < 6; ++i)
    {
      fprintf (stderr, "play boop %d\n", i);
      messenger.SendPlayBoop(i);
      sleep (1);
    }

  return 0;
}
