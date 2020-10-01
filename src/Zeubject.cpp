
#include <Zeubject.h>


namespace charm {


Zeubject::Zeubject ()  :  OmNihil (), name (NULL)
{ }

Zeubject::Zeubject (const std::string &nm)  :  OmNihil (),
                                               name (new std::string (nm))
{ }


static std::string bereft_str ("");


const std::string &Zeubject::Name ()  const
{ return name  ?  *name  :  bereft_str; }

void Zeubject::SetName (const std::string &n)
{ if (! name)
    name = new std::string (n);
  else
    *name = n;
}


i64 Zeubject::Inhale (i64 /*ratch*/, f64 /*thyme*/)
{ return 0; }


}
