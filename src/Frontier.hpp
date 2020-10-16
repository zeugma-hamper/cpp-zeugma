#ifndef VAST_TRACKS_OF_LAND
#define VAST_TRACKS_OF_LAND

#include <GeomFumble.h>
#include <Vect.h>

#include <base_types.hpp>
#include <class_utils.hpp>

namespace charm
{
class Node;

class Frontier
{
 public:
  friend Node;

  Frontier ();
  Frontier (Node *_node);
  virtual ~Frontier ();

  virtual AABB GetLocalAABB  () const;
  virtual AABB GetGlobalAABB () const;

  virtual bool CheckHit (Ray const &_ray, Vect *_hit_pt) const = 0;

 protected:
  Node *m_node; //non-owning
};


class RectangleFrontier : public Frontier
{
 public:
  RectangleFrontier (Vect const &_bl, Vect const &_tr, Vect const &_norm);

  AABB GetLocalAABB  () const override;
  AABB GetGlobalAABB () const override;
  bool CheckHit (Ray const &_ray, Vect *_hit_pt) const override;

 protected:
  Vect m_bl;
  Vect m_tr;
  Vect m_norm;
};

}


#endif //VAST_TRACKS_OF_LAND
