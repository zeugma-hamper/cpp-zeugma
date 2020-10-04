
#ifndef ZE_YOWL_EVENT_CAN_BE_HEARD_ALL_THE_WAY_IN_THE_RESTROOMS_OF_ATLANTIS
#define ZE_YOWL_EVENT_CAN_BE_HEARD_ALL_THE_WAY_IN_THE_RESTROOMS_OF_ATLANTIS


#include <ZeEvent.h>


namespace charm  {


class ZEYowlEvent  :  public ZeEvent
{ ZE_EVT_TUMESCE (ZEYowl, Ze);

  std::string utt;
  i32 rept_ord;

  ZEYowlEvent ();
  ZEYowlEvent (std::string_view u);
  ZEYowlEvent (std::string_view u, i32 repetition_ordinal);

  ZEYowlEvent (const ZEYowlEvent &yoe);

  const std::string &Utterance ()  const;
  void SetUtterance (std::string_view u);

  i32 RepetitionOrdinal ()  const;
  void SetRepetitionOrdinal (i32 ro);
};



class ZEYowlAppearEvent  :  public ZEYowlEvent
{ ZE_EVT_TUMESCE (ZEYowlAppear, ZEYowl);

  ZEYowlAppearEvent ()  :  ZEYowlEvent ()
    { }
  ZEYowlAppearEvent (std::string_view u)  :  ZEYowlEvent (u)
    { }
  ZEYowlAppearEvent (std::string_view u, i32 repetition_ordinal)
     :  ZEYowlEvent (u, repetition_ordinal)
    { }

  ZEYowlAppearEvent (const ZEYowlEvent &yoe)  :  ZEYowlEvent (yoe)
    { SetRepetitionOrdinal (0); }
};



class ZEYowlRepeatEvent  :  public ZEYowlEvent
{ ZE_EVT_TUMESCE (ZEYowlRepeat, ZEYowl);

  ZEYowlRepeatEvent ()  :  ZEYowlEvent ()
    { }
  ZEYowlRepeatEvent (std::string_view u)  :  ZEYowlEvent (u)
    { }
  ZEYowlRepeatEvent (std::string_view u, i32 repetition_ordinal)
     :  ZEYowlEvent (u, repetition_ordinal)
    { }

  ZEYowlRepeatEvent (const ZEYowlEvent &yoe, i32 rep)  :  ZEYowlEvent (yoe)
    { SetRepetitionOrdinal (rep); }
};



class ZEYowlVanishEvent  :  public ZEYowlEvent
{ ZE_EVT_TUMESCE (ZEYowlVanish, ZEYowl);

  ZEYowlVanishEvent ()  :  ZEYowlEvent ()
    { SetRepetitionOrdinal (-1); }
  ZEYowlVanishEvent (std::string_view u)  :  ZEYowlEvent (u, -1)
    { }

  ZEYowlVanishEvent (const ZEYowlEvent &yoe)  :  ZEYowlEvent (yoe)
    { SetRepetitionOrdinal (-1); }
};



class ZEYowlPhagy
 :  public ZEYowlAppearEvent::ZEYowlAppearPhage,
    public ZEYowlRepeatEvent::ZEYowlRepeatPhage,
    public ZEYowlVanishEvent::ZEYowlVanishPhage
{ };


}  // namespace charm having a lovely chat with jimmy hoffa


#endif //ZE_YOWL_EVENT_CAN_BE_HEARD_ALL_THE_WAY_IN_THE_RESTROOMS_OF_ATLANTIS
