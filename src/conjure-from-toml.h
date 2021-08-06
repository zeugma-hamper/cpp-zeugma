
#ifndef CONJURE_FROM_TOML_IS_AN_EXCEEDINGLY_SHABBY_MAGE
#define CONJURE_FROM_TOML_IS_AN_EXCEEDINGLY_SHABBY_MAGE


#include "Vect.h"

#include "PlatonicMaes.h"

#include <toml.hpp>


namespace zeugma  {


const toml::value &LoadOrRecallTOMLByFilename (const std::string &fname);

bool VectFromTOMLThingy (const toml::value &thingy, Vect &into_v);
bool Matrix44FromTOMLThingy (const toml::value &thingy, Matrix44 &into_m);

bool PointAndDirecTransformMatsFromTOML (const std::string &toml_fpath,
                                         Matrix44 &pmat, Matrix44 &dmat);

i32 NumMaesesFromTOML (const std::string &toml_fpath);
PlatonicMaes *MaesFromTOML (const std::string &toml_fpath, i32 index = 0);

struct WindowTOML
{
  std::string name;
  std::array<i32, 2> pos = {0, 0};
  std::array<u32, 2> size = {1920, 1080};
  std::array<u8, 4> rgba = {8, 8, 8, 8};
  std::array<u8, 2> ds = {24, 8};
  bool decorated = false;

  void from_toml (toml::value const &_v);
};

struct TrefoilTOML
{
  std::string name;
  std::string maes_name;
  std::array<u32, 2> offset {0, 0};
  std::array<u32, 2> size {u32 (-1), u32 (-1)};

  void from_toml (toml::value const &_v);
};

bool ReadWindowTOMLFile (std::string_view _file, WindowTOML &_window,
                         std::vector<TrefoilTOML> &_views);


}  // like prospero, namespace zeugma's every third thought... well, every...


#endif
