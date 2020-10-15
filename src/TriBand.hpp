#ifndef MAXIMUM_HULA_HOOPING_SKILLS
#define MAXIMUM_HULA_HOOPING_SKILLS

#include <Node.hpp>

namespace charm
{

class FilmInfo;

class TriBand final : public Node
{
 public:
  TriBand (f64 _width, f64 _height, std::vector<FilmInfo> const &_films);

 protected:
};

}

#endif
