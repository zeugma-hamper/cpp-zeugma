
#ifndef ZOFT_MAY_OR_MAY_NOT_BE_THE_NEW_SOFT
#define ZOFT_MAY_OR_MAY_NOT_BE_THE_NEW_SOFT


#include "Zeubject.h"
#include "MotherTime.h"
#include "ProtoZoftThingGuts.h"
#include <Vect.h>
#include <ZeColor.h>

#include <vector>
#include <algorithm>

#include <math.h>


// #define ZOFT_MAYBINHALE(ratch, thyme)


namespace charm  {


namespace Zoft
{ extern MotherTime zeit; }


template <typename T>
class ZoftThing
{ public:

  using ThisHereZoftType = ZoftThing <T>;

  template <typename TT>
    class ZGuts  :  public ProtoZoftThingGuts
      { public:
        union
          { ZoftThing <TT> *zt;
            std::vector <ZoftThing <TT> *> *zts;
          } hostesque;

        ZGuts ()  :  ProtoZoftThingGuts ()
          { hostesque.zt = NULL;
            EnrollForMassBreathing ();
          }
        ~ZGuts ()
          { if (num_hosts > 1)
              delete hostesque.zts;
            WithdrawFromMassBreathing ();
          }

        void AppendHost (ZoftThing <TT> *z)
          { if (! z)
              return;
            if (num_hosts == 0)
              hostesque.zt = z;
            else if (num_hosts == 1)
              { if (hostesque.zt == z)
                  return;
                ZoftThing <TT> *tmp = hostesque.zt;
                hostesque.zts = new std::vector <ZoftThing <TT> *> ();
                hostesque.zts -> push_back (tmp);
                hostesque.zts -> push_back (z);
              }
            else
              { if (std::find (hostesque.zts -> begin (),
                               hostesque.zts -> end(), z)
                    !=  hostesque.zts -> end ())
                  return;  // already present. see how easy finding things is?
                hostesque.zts -> push_back (z);
              }
            ++num_hosts;
          }

        void RemoveHost (ZoftThing <TT> *z)
          { if (! z  ||  num_hosts == 0)
              return;
            if (num_hosts == 1)
              { if (hostesque.zt != z)
                  return;
                hostesque.zt = NULL;
              }
            else
              { auto it = std::find (hostesque.zts -> begin (),
                                     hostesque.zts -> end (), z);
                if (it == hostesque.zts -> end ())
                  return;
                hostesque.zts -> erase (it);
                if (num_hosts == 2)
                  { ZoftThing <TT> *tmp = hostesque.zts -> at (0);
                    delete hostesque.zts;
                    hostesque.zt = tmp;
                  }
              }
            if (--num_hosts  ==  0)
              delete this;
          }

        void PuppeteerHosts (const TT &v)
          { if (num_hosts == 1)
              hostesque.zt->val = v;
            else if (num_hosts > 1)
              { for (ZoftThing <TT> *z  :  *hostesque.zts)
                  z->val = v;
              }
          }

        i64 Inhale (i64 ratch, f64 thyme)  override
          { return 0; }
      };

  T val;
  ZGuts <T> *guts;
  bool just_changed, nomo_change;

  template <typename TT>
    class LatchGuts  :  public ZoftThing<TT>::template ZGuts<TT>
      { public:
        TT latchval;

        LatchGuts ()  :  ZoftThing<TT>::template ZGuts<TT> (),
           latchval (TT (0.0))
          { }
        LatchGuts (const TT &lv)
           :  ZoftThing<TT>::template ZGuts<TT> (), latchval (lv)
          { }

        i64 Inhale (i64 ratch, f64 thyme)  override
          { this -> PuppeteerHosts (latchval);
            return 0;
          }
      };

  ZoftThing ()
     :  guts (NULL), just_changed (true), nomo_change (false)
    { }
  ZoftThing (const T &v)
     :  val (v), guts (NULL), just_changed (true), nomo_change (false)
    { }
  ZoftThing (const ZoftThing &z)
     :  val (z.val), guts (NULL), just_changed (true),
        nomo_change (z.nomo_change)
    { InstallGuts (z.guts); }

  ~ZoftThing ()
    { if (guts)
        guts -> RemoveHost (this);
    }

  ZoftThing &Set (const T &v)
    { if (! guts)
        val = v;
      else if (LatchGuts <T> *lg = dynamic_cast <LatchGuts <T> *> (guts))
        lg->latchval = v;
      else
        return *this;
      just_changed = true;
      return *this;
    }

  ZoftThing &SetHard (const T &v)
    { if (! guts)
        val = v;
      else if (LatchGuts <T> *lg = dynamic_cast <LatchGuts <T> *> (guts))
        val = lg->latchval = v;
      else
        return *this;
      just_changed = true;
      return *this;
    }

  ZoftThing &operator = (const T &v)
    { return Set (v); }
  ZoftThing &operator = (ZoftThing &z)
    { val = z.val;
      nomo_change = z.nomo_change;
      InstallGuts (z.guts);
      return *this;
    }

  ZoftThing &BecomeLike (ZoftThing &z)
    { if (&z  ==  this)
        return *this;
      val = z.val;
      nomo_change = z.nomo_change;
      if (! z.guts)
        z . MakeBecomeLikable ();
      InstallGuts (z.guts);
      return *this;
    }
  ZoftThing &BecomeLike (const ZoftThing &z)
    { if (&z  ==  this)
        return *this;
      val = z.val;
      nomo_change = z.nomo_change;
      InstallGuts (z.guts);
      return *this;
    }

  ZoftThing &MakeBecomeLikable ()
    { if (! guts)
        InstallGuts (new LatchGuts <T> (val));
      return *this;
    }

  ZoftThing &MakeGutless ()
    { if (guts)
        { guts -> RemoveHost (this);  guts = NULL; }
      return *this;
    }


  operator const T & ()  const
    { return val; }

  const T & Val ()  const
    { return val; }

  ZGuts <T> *Guts ()
    { return guts; }

  // this next, and the following method, are fairly useless but are
  // provided here for resonance with (and completeness with respect
  // to) the identical definitions in derived classes...
  using GutsTyp = ZGuts <T>;

  GutsTyp *GutsIfOrigType ()
    { return dynamic_cast <GutsTyp *> (this -> Guts ()); }

  void InstallGuts (ZGuts <T> *g)
    { if (g == guts)
        return;
      if (guts)
        { guts -> RemoveHost (this);
          // if last host delete guts?
        }
      guts = g;
      if (guts)
        guts -> AppendHost (this);
      // just_changed = true;  nomo_change = false;  // assuming guts does this
    }

  i64 Inhale (i64 ratch, f64 thyme)
    { if (nomo_change)
        { if (just_changed)
            just_changed = false;
          return 0;
        }
      if (guts  &&  (ratch < 0  ||  guts->rat_fresh < ratch))
        guts -> Inhale (ratch, thyme);
      return 0;
    }
};


using ZoftFloat = ZoftThing <f64>;
using ZoftVect = ZoftThing <Vect>;
using ZoftColor = ZoftThing <ZeColor>;

extern const ZoftFloat ZoftFloat_zero;
extern const ZoftVect ZoftVect_zero;


}  // no ashes; just dust for namespace charm


#endif
