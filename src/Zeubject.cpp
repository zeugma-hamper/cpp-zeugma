
#include <Zeubject.h>

namespace charm {

Zeubject::Zeubject ()  :  OmNihil (), name (NULL)
{ }

Zeubject::Zeubject (std::string_view n)  :  OmNihil (), name (n)
{ }

const std::string &Zeubject::Name ()  const
{ return name; }

void Zeubject::SetName (std::string_view n)
{
  name = n;
}

i64 Zeubject::Inhale (i64 /*steppe*/, f64 /*thyme*/)
{
  return 0;
}

}
