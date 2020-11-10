
#include "ZEYowlEvent.h"

#include <MultiSprinkler.hpp>

namespace charm  {


ZEYowlEvent::ZEYowlEvent ()  :  ZeEvent (), rept_ord (0)
{ }

ZEYowlEvent::ZEYowlEvent (std::string_view u)
 :  ZeEvent (), utt (u), rept_ord (0)
{ }

ZEYowlEvent::ZEYowlEvent (std::string_view u, i32 repetition_ordinal)
 :  ZeEvent (), utt (u), rept_ord (repetition_ordinal)
{ }

ZEYowlEvent::ZEYowlEvent (const ZEYowlEvent &yoe)
 :  ZeEvent (yoe), utt (yoe.utt), rept_ord (yoe.rept_ord)
{ }


const std::string &ZEYowlEvent::Utterance ()  const
{ return utt; }

void ZEYowlEvent::SetUtterance (std::string_view u)
{ utt = u; }


i32 ZEYowlEvent::RepetitionOrdinal ()  const
{ return rept_ord; }

void ZEYowlEvent::SetRepetitionOrdinal (i32 ro)
{ rept_ord = ro; }



void AppendYowlPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage)
{ if (! _ms || ! _phage)
    return;

  _ms -> AppendPhage <ZEYowlRepeatEvent> (_phage);
  _ms -> AppendPhage <ZEYowlAppearEvent> (_phage);
  _ms -> AppendPhage <ZEYowlVanishEvent> (_phage);
}

void RemoveYowlPhage (MultiSprinkler *_ms, ch_ptr<OmNihil> const &_phage)
{ if (! _ms || ! _phage)
    return;

  _ms -> RemovePhage <ZEYowlRepeatEvent> (_phage);
  _ms -> RemovePhage <ZEYowlAppearEvent> (_phage);
  _ms -> RemovePhage <ZEYowlVanishEvent> (_phage);
}


}
