
#include <Zeubject.h>


namespace charm {


Zeubject::Zeubject ()  :  OmNihil (), sac (NULL)
{ }

Zeubject::Zeubject (const std::string &nm)  :  OmNihil (), sac (NULL)
{ SetName (nm); }

Zeubject::~Zeubject ()
{ if (sac)
    delete sac;
}


ZeSac *Zeubject::UnsecuredSac ()
{ return sac; }

ZeSac *Zeubject::AssuredSac ()
{ return sac  ?  sac  :  (sac = new ZeSac); }




const std::string &Zeubject::Name ()
{ if (ZeSac *ess = UnsecuredSac ())
    return ess -> Name ();
  return ZeSac::bereft_str;
}

void Zeubject::SetName (const std::string &n)
{ AssuredSac () -> SetName (n); }


IronLung *Zeubject::Breather ()
{ ZeSac *ess = UnsecuredSac ();
  return ess  ?  ess -> Breather ()  :  NULL;
}

void Zeubject::SetBreather (IronLung *br)
{ AssuredSac () -> SetBreather (br); }


i64 Zeubject::Inhale (i64 /*ratch*/, f64 /*thyme*/)
{ return 0; }



std::string ZeSac::bereft_str ("");


}
