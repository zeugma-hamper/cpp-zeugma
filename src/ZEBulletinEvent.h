
#ifndef ZE_BULLETIN_EVENT_MAY_OR_MAY_NOT_BE_ALL_POINTS
#define ZE_BULLETIN_EVENT_MAY_OR_MAY_NOT_BE_ALL_POINTS


#include "ZeEvent.h"


namespace charm  {


class ZEBulletinEvent  :  public ZeEvent
{ ZE_EVT_TUMESCE (ZEBulletin, Ze);

  struct ObjPhrase { std::string tag;  OmNihil *obj; };
  struct StrPhrase { std::string tag;  std::string str; };

  std::string headline;
  std::vector <ObjPhrase> o_phrs;
  std::vector <StrPhrase> s_phrs;

  ZEBulletinEvent ()  :  ZeEvent ()
    { }
  ZEBulletinEvent (const std::string &hdln)  :  ZeEvent ()
    { SetHeadline (hdln); }
  ZEBulletinEvent (const std::string &hdln,
                   const std::string &tag, OmNihil *ob)  :  ZeEvent ()
    { SetHeadline (hdln);  AppendObjPhrase (tag, ob); }
  ZEBulletinEvent (const std::string &hdln,
                   const std::string &tag, const std::string &s)  :  ZeEvent ()
    { SetHeadline (hdln);  AppendStrPhrase (tag, s); }

  const std::string &Headline ()  const
    { return headline; }
  void SetHeadline (const std::string &h)
    { headline = h; }

  bool Says (const std::string &s)  const
    { return headline  ==  s; }
  bool BeginsToSay (const std::string &s)  const
    { return headline . find (s)  ==  0; }

  u32 NumObjPhrases ()  const;
  const ObjPhrase *NthObjPhrase (u32 ind)  const;
  const ObjPhrase *FindObjPhrase (const std::string &srch)  const;
  const ObjPhrase *PrefixFindObjPhrase (const std::string &srch)  const;

  OmNihil *ObjByTag (const std::string &s)  const
    { if (const ObjPhrase *p = FindObjPhrase (s))  return p->obj;
      return NULL;
    }
  OmNihil *ObjByTagPrefix (const std::string &s)  const
    { if (const ObjPhrase *p = PrefixFindObjPhrase (s))  return p->obj;
      return NULL;
    }

  void AppendObjPhrase (const std::string &sbj, OmNihil *obj = NULL);
  void AppendObjPhrase (const ObjPhrase &phr);


  u32 NumStrPhrases ()  const;
  const StrPhrase *NthStrPhrase (u32 ind)  const;
  const StrPhrase *FindStrPhrase (const std::string &srch)  const;
  const StrPhrase *PrefixFindStrPhrase (const std::string &srch)  const;

  void AppendStrPhrase (const std::string &sbj, const std::string &s);
  void AppendStrPhrase (const StrPhrase &phr);
};


class ZEBulletinPhagy
 :  public ZEBulletinEvent::ZEBulletinPhage
{ };


class MultiSprinkler;
void AppendBulletinPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage);
void RemoveBulletinPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage);

template <typename T>
 void AppendBulletinPhage (MultiSprinkler *_ms, ch_ptr<T> const &_phage)
{ AppendBulletinPhage (_ms, static_ch_cast<OmNihil> (_phage)); }

template <typename T>
 void RemoveBulletinPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage)
{ RemoveBulletinPhage (_ms, static_ch_cast<OmNihil> (_phage)); }


using ZEBObjPhrase = ZEBulletinEvent::ObjPhrase;
using ZEBStrPhrase = ZEBulletinEvent::StrPhrase;

}


#endif
