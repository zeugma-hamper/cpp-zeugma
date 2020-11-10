
#ifndef COME_ON_ITS_OK_LET_IT_ALL_OUT_SHHHHHH
#define COME_ON_ITS_OK_LET_IT_ALL_OUT_SHHHHHH


#include <ZePublicWaterWorks.hpp>

#include <ZESpatialEvent.h>
#include <ZEYowlEvent.h>


extern "C"
{
  struct GLFWwindow;
}


namespace charm
{


class Bolex;


struct MouseState
{
  ZESpatialEvent event;
  u64 button_state = 0;
  bool valid = false;
};


class GLFWWaterWorks : public ZePublicWaterWorks
{
 public:
  GLFWWaterWorks (GLFWwindow *window);

  void Drain (MultiSprinkler *_ms) override;

  // called by CreateZE* methods
  void SpraySpatialMove   (ZESpatialMoveEvent *_zsme);
  void SpraySpatialHarden (ZESpatialHardenEvent *_zshe);
  void SpraySpatialSoften (ZESpatialSoftenEvent *_zsse);

  // called by GLFW callbacks in cpp file
  void SprayYowlAppear (ZEYowlAppearEvent *_zyae);
  void SprayYowlRepeat (ZEYowlRepeatEvent *_zyre);
  void SprayYowlVanish (ZEYowlVanishEvent *_zyve);

  // called by GLFW mouse callbacks
  void CreateZEMoveFromGLFW (GLFWwindow *_window, f64 _x, f64 _y);
  void CreateZEPressureFromGLFW (GLFWwindow *wind, int butt,
                                 int actn, int mods);

 protected:
  MultiSprinkler *m_temp_sprinkler;
  MouseState m_mouse_state;
};

}

#endif
