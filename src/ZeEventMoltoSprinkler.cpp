#include <ZeEventMoltoSprinkler.hpp>

namespace charm
{

MoltoSprinkler::MoltoSprinkler ()
{
}

MoltoSprinkler::~MoltoSprinkler ()
{
  for (auto p : sprinkler_map)
    delete p.second;

  sprinkler_map.clear();
}


}
