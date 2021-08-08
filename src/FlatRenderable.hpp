
//
// (c) treadle & loam, provisioners llc
//

#ifndef WIDE_AND_HIGH_HOOOOO_NOW
#define WIDE_AND_HIGH_HOOOOO_NOW


#include <Renderable.hpp>


namespace zeugma  {


class FlatRenderable : public Renderable
{
 public:
  FlatRenderable ();
  FlatRenderable (Node *_node);

  // assuming this is going to be updated once softs are installed
  void SetWidth (f32 _width);
  f32  GetWidth () const;

  // assuming this is going to be updated once softs are installed
  void SetHeight (f32 _height);
  f32  GetHeight () const;

 protected:
  f32 m_width;
  f32 m_height;
};


}


#endif  //WIDE_AND_HIGH_HOOOOO_NOW
