#include <class_utils.hpp>

namespace charm
{

class Application
{
 public:
  Application () = default;
  virtual ~Application () = default;

  CHARM_DELETE_MOVE_COPY(Application);

  bool InstallDefaultSignalHandlers ();

  static void StopRunning ();

  virtual bool StartUp ();
  //empty, but provided to avoid abstract class
  virtual bool Update ();
  //empty, but provided to avoid abstract class
  virtual bool ShutDown ();

  //installs signal handler, calls initialize, loops update, then calls shutdown
  virtual void Run ();
};

}
