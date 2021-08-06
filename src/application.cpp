
#include <application.hpp>

#include <assert.h>
#include <signal.h>
#include <string.h>


namespace zeugma  {


static bool s_keep_running = true;

bool Application::StartUp ()
{
  InstallDefaultSignalHandlers ();

  return true;
}

bool Application::RunOneCycle ()
{
  return true;
}

bool Application::ShutDown ()
{
  return true;
}

void Application::Run ()
{
  while (RunOneCycle () && s_keep_running);

  ShutDown ();
}

bool Application::DoWhatThouWilt (i64 ratch, f64 thyme)
{ return true; }

static void s_term_int_handler (int, siginfo_t *, void *)
{
  s_keep_running = false;
}

bool Application::InstallDefaultSignalHandlers ()
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


MultiSprinkler &Application::GetSprinkler ()
{
  return m_event_sprinkler;
}


void Application::StopRunning ()
{
  s_keep_running = false;
}

}
