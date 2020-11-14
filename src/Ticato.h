
#ifndef TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM
#define TICATO_IS_IN_THE_MIDDLE_OF_ANY_CINEMATIC_ATOM


#include "Node.hpp"
#include "Renderable.hpp"

#include "Matte.hpp"

#include "Zeubject.h"


using namespace charm;


class Ticato  :  public Zeubject
{ public:

  Node *no;
  Renderable *re;
  ZoftVect sca, loc;

  Ticato (std::vector <FilmInfo> &fimmz, i64 which_fimm = -1,
          i64 which_clip = -1);
};


#endif
