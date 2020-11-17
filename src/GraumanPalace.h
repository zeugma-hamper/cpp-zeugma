
#ifndef GRAUMAN_PALACE_FOR_THE_VERY_SIDNEYEST_SPECTACLE
#define GRAUMAN_PALACE_FOR_THE_VERY_SIDNEYEST_SPECTACLE


#include "Zeubject.h"

#include "PlatonicMaes.h"

#include "ZEBulletinEvent.h"
#include "ZESpatialEvent.h"
#include "ZEYowlEvent.h"

#include <Node.hpp>
#include <Matte.hpp>
#include <VideoRenderable.hpp>

#include <vector>


using namespace charm;


class SilverScreen  :  public Node
{ public:
  const FilmInfo &finf;

  SilverScreen (const FilmInfo &fi)  :  Node (), finf (fi)
    { }
};


class GraumanPalace  :  public Zeubject, public Node,
                        public ZESpatialPhagy,
                        public ZEYowlPhagy,
                        public ZEBulletinPhagy
{ public:
  PlatonicMaes *backing_maes;
  std::vector <VideoRenderable *> movies;
  std::vector <Node *> screens;

  GraumanPalace ();

  void ImportExhibitionRoster (const std::vector <FilmInfo> &fimmz);
};


#endif
