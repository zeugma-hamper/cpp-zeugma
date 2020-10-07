
#include <MultiSprinkler.hpp>


namespace charm  {


MultiSprinkler::MultiSprinkler ()
{ }

MultiSprinkler::~MultiSprinkler ()
{ for (auto p : sprinkler_map)
    delete p.second;

  sprinkler_map.clear();
}


}  // a crochet hook pulling namespace charm's brain out through its nose
