#include <application.hpp>

#include <assert.h>
#include <signal.h>
#include <string.h>

namespace charm
{
static bool s_keep_running = true;

bool application::start_up ()
{
  return true;
}

bool application::update ()
{
  return true;
}

bool application::shut_down ()
{
  return true;
}

void application::run ()
{
  if (! install_default_signal_handlers() ||
      ! start_up())
    return;

  while (s_keep_running)
    s_keep_running = update () && s_keep_running;

  shut_down ();
}

static void s_term_int_handler (int, siginfo_t *, void *)
{
  s_keep_running = false;
}

bool application::install_default_signal_handlers ()
{
  s_keep_running = true;

  struct sigaction sa;
  memset (&sa, 0, sizeof (struct sigaction));

  sa.sa_sigaction = s_term_int_handler;
  sa.sa_flags = SA_SIGINFO;
  //block all signals while handling signal
  sigfillset (&sa.sa_mask);

  assert (0 == sigaction (SIGTERM, &sa, nullptr));
  assert (0 == sigaction (SIGINT, &sa, nullptr));

  return true;
}

void application::stop_running ()
{
  s_keep_running = false;
}

}
