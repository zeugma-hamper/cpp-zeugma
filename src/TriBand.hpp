
#ifndef MAXIMUM_HULA_HOOPING_SKILLS
#define MAXIMUM_HULA_HOOPING_SKILLS


#include <Node.hpp>

#include "PlatonicMaes.h"

#include "LatchZoft.h"


namespace charm  {


class FilmInfo;


class CineAtom  :  public Node
{ public:
  ZoftVect loc;

  CineAtom ()  :  Node ()
    { }
};


class ElementsBand final : public Node
{
 public:
  ElementsBand (f64 _band_width, f64 _band_height,
                std::vector<FilmInfo> const &_films,
                PlatonicMaes &maes, const Vect &cntr);

};

class TriBand final : public Node
{
 public:
  TriBand (f64 _width, f64 _height, std::vector<FilmInfo> const &_films);

 protected:
};


}


#endif
