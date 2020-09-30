
#include "ZEYowlEvent.h"

namespace charm
{

ZEYowlEvent::ZEYowlEvent ()  :  ZeEvent (), rept_ord (0)
{ }

ZEYowlEvent::ZEYowlEvent (std::string_view u)  :  ZeEvent (), utt (u), rept_ord (0)
{ }

ZEYowlEvent::ZEYowlEvent (std::string_view u, i32 repetition_ordinal)  :  ZeEvent (),
   utt (u), rept_ord (repetition_ordinal)
{ }

const std::string &ZEYowlEvent::Utterance ()  const
{
  return utt;
}

void ZEYowlEvent::SetUtterance (std::string_view u)
{
  utt = u;
}

i32 ZEYowlEvent::RepetitionOrdinal ()  const
{
  return rept_ord;
}

void ZEYowlEvent::SetRepetitionOrdinal (i32 ro)
{
  rept_ord = ro;
}

}
