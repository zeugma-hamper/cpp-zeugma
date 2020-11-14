
#include "ZeColor.h"


using namespace charm;


ZeColor &ZeColor::Set (f32 gray)
{ r = g = b = gray;  a = 1.0;  return *this; }

ZeColor &ZeColor::SetNoAlpha (f32 gray)
{ r = g = b = gray;  return *this; }

ZeColor &ZeColor::Set (f32 gray, f32 alph)
{ r = g = b = gray;  a = alph;  return *this; }

ZeColor &ZeColor::Set (f32 arr, f32 gee, f32 bee)
{ r = arr;  g = gee;  b = bee;  a = 1.0;  return *this; }

ZeColor &ZeColor::SetNoAlpha (f32 arr, f32 gee, f32 bee)
{ r = arr;  g = gee;  b = bee;  return *this; }

ZeColor &ZeColor::Set (f32 arr, f32 gee, f32 bee, f32 alp)
{ r = arr;  g = gee;  b = bee;  a = alp;  return *this; }
