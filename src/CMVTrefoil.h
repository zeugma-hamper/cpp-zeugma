
#ifndef WHERE_WORLDS_COLLIDE
#define WHERE_WORLDS_COLLIDE


#include <base_types.hpp>
#include <bgfx_utils.hpp>

#include <Zeubject.h>


namespace charm  {


class Bolex;
class PlatonicMaes;
class Layer;

Bolex *CameraFromMaes (const PlatonicMaes &m);


struct BGFXView
{ std::string name;
  i32 view_id = 0;
  bgfx::FrameBufferHandle fb_handle = BGFX_INVALID_HANDLE;
  i64 fb_pix_l = 0, fb_pix_b = 0;
  i64 fb_pix_r = 0, fb_pix_t = 0;

  i64 GetWidth ()  const
    { return fb_pix_r - fb_pix_l; }

  i64 GetHeight ()  const
    { return fb_pix_t - fb_pix_b; }

  u16 ViewID ()  const
    { return u16 (view_id); }
};


class CMVTrefoil  :  public Zeubject
{ public:
  Bolex *cam;
  PlatonicMaes *maes;
  BGFXView view;
  std::vector <Layer *> layers;

  CMVTrefoil (PlatonicMaes *_maes)  :  Zeubject (),
                                       cam (nullptr), maes (_maes), view ()
    { }

  CMVTrefoil (Bolex *_cam, PlatonicMaes *_maes)  :  Zeubject (),
                                                    cam (_cam), maes (_maes),
                                                    view ()
    { }

  i64 Inhale (i64 ratch, f64 thyme)  override;
};


}


#endif  //WHERE_WORLDS_COLLIDE
