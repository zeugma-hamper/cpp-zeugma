
#include "ZEBulletinEvent.h"

#include <MultiSprinkler.hpp>


namespace zeugma  {


u32 ZEBulletinEvent::NumObjPhrases ()  const
{ return o_phrs . size (); }

const ZEBulletinEvent::ObjPhrase
  *ZEBulletinEvent::NthObjPhrase (u32 ind)  const
{ if (ind  >=  NumObjPhrases ())
    return NULL;
  return &o_phrs[ind];
}

const ZEBulletinEvent::ObjPhrase
  *ZEBulletinEvent::FindObjPhrase (const std::string &srch)  const
{ for (const ObjPhrase &phr  :  o_phrs)
    if (phr.tag  ==  srch)
      return &phr;
  return NULL;
}

const ZEBulletinEvent::ObjPhrase
  *ZEBulletinEvent::PrefixFindObjPhrase (const std::string &srch)  const
{ for (const ObjPhrase &phr  :  o_phrs)
    if (phr.tag . find (srch)  ==  0)
      return &phr;
  return NULL;
}

void ZEBulletinEvent::AppendObjPhrase (const std::string &sbj, OmNihil *obj)
{ o_phrs . push_back (ObjPhrase {sbj, obj}); }

void ZEBulletinEvent::AppendObjPhrase (const ObjPhrase &phr)
{ o_phrs . push_back (phr); }



u32 ZEBulletinEvent::NumStrPhrases ()  const
{ return o_phrs . size (); }

const ZEBulletinEvent::StrPhrase
  *ZEBulletinEvent::NthStrPhrase (u32 ind)  const
{ if (ind  >=  NumStrPhrases ())
    return NULL;
  return &s_phrs[ind];
}

const ZEBulletinEvent::StrPhrase
  *ZEBulletinEvent::FindStrPhrase (const std::string &srch)  const
{ for (const StrPhrase &phr  :  s_phrs)
    if (phr.tag  ==  srch)
      return &phr;
  return NULL;
}

const ZEBulletinEvent::StrPhrase
  *ZEBulletinEvent::PrefixFindStrPhrase (const std::string &srch)  const
{ for (const StrPhrase &phr  :  s_phrs)
    if (phr.tag . find (srch)  ==  0)
      return &phr;
  return NULL;
}

void ZEBulletinEvent::AppendStrPhrase (const std::string &sbj,
                                       const std::string &s)
{ s_phrs . push_back (StrPhrase {sbj, s}); }

void ZEBulletinEvent::AppendStrPhrase (const StrPhrase &phr)
{ s_phrs . push_back (phr); }



void AppendBulletinPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage)
{ if (! _ms  ||  ! _phage)
    return;
  _ms -> AppendPhage <ZEBulletinEvent> (_phage);
}


void RemoveBulletinPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage)
{ if (! _ms  ||  ! _phage)
    return;
  _ms -> RemovePhage <ZEBulletinEvent> (_phage);
}


}
