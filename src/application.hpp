#include <utils.hpp>

namespace charm
{

class application
{
 public:
  application () = default;
  virtual ~application () = default;

  CHARM_DELETE_MOVE_COPY(application);

  bool install_default_signal_handlers ();

  static void stop_running ();

  virtual bool start_up ();
  //empty, but provided to avoid abstract class
  virtual bool update ();
  //empty, but provided to avoid abstract class
  virtual bool shut_down ();

  //installs signal handler, calls initialize, loops update, then calls shutdown
  virtual void run ();
};

}
