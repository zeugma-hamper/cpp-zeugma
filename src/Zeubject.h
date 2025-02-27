
//
// (c) treadle & loam, provisioners llc
//

#ifndef ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
#define ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK


#include <base_types.hpp>
#include <OmNihil.h>

#include <string>


namespace zeugma  {


struct ZeSac;

class IronLung;


class Zeubject  :  public OmNihil
{ public:

  ZeSac *sac;
  i64 rat_fresh;

  Zeubject ();
  Zeubject (const std::string &nm);
  ~Zeubject ()  override;

  ZeSac *UnsecuredSac ();
  ZeSac *AssuredSac ();

  const std::string &Name ();
  void SetName (const std::string &n);

  IronLung *Breather ();
  void SetBreather (IronLung *br);

  // empty implementation for the moment, returns 0
  bool OughtToInhale (i64 ratch)
    { if (ratch < 0)
        return true;
      if (rat_fresh >= ratch)
        return false;
      rat_fresh = ratch;
      return true;
    }
  virtual i64 Inhale (i64 ratch, f64 thyme);
};


struct ZeSac
{ std::string *name;
  IronLung *irlu;

  static std::string bereft_str;

  ZeSac ()  :  name (NULL), irlu (NULL)
    { }
  ~ZeSac ()
    { if (name)
        delete name;
    }

  const std::string &Name ()  const
    { return name  ?  *name  :  bereft_str; }
  void SetName (const std::string &nm)
    { if (! name)
        name = new std::string (nm);
      else
        *name = nm;
    }

  IronLung *Breather ()  const
    { return irlu; }
  void SetBreather (IronLung *br)
    { irlu = br; }
};


}  // another glorious martyrdom for namespace zeugma


#endif  //ZEUBJECT_IS_NO_SYLLEPTICAL_WALK_IN_THE_PORK
