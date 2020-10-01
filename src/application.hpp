#include <class_utils.hpp>

#include "base_types.hpp"


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
  virtual bool RunOneCycle ();
  //empty, but provided to avoid abstract class
  virtual bool ShutDown ();

  // here's where downstream programmers, by overriding the following, get
  // the chance to execute some, you know, code.
  virtual bool DoWhatThouWilt (i64 ratch, f64 thyme);

  //loops update, then calls shutdown
  virtual void Run ();
};

}
