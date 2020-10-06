
#ifndef CONJURE_FROM_TOML_IS_AN_EXCEEDINGLY_SHABBY_MAGE
#define CONJURE_FROM_TOML_IS_AN_EXCEEDINGLY_SHABBY_MAGE


#include <toml.hpp>

#include "Vect.h"

#include "PlatonicMaes.h"


namespace charm  {


bool VectFromTOMLThingy (const toml::value &thingy, Vect &into_v);

i32 NumMaesesFromTOML (const std::string &toml_fpath);
PlatonicMaes *MaesFromTOML (const std::string &toml_fpath, i32 index = 0);


}  // like prospero, namespace charm's every third though... well, every thought


#endif
