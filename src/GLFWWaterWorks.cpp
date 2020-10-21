#include <GLFWWaterWorks.hpp>

#include <Bolex.h>
#include <CMVTrefoil.h>
#include <GraphicsApplication.hpp>
#include <MultiSprinkler.hpp>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <unordered_map>

namespace charm
{

static GLFWWaterWorks *s_rmww_instance {nullptr};

static void glfw_mousepos_callback (GLFWwindow *win, double x, double y)
{
  s_rmww_instance->CreateZEMoveFromGLFW(win, x, y);
}


static void glfw_mousebutt_callback (GLFWwindow *wind, int butt,
                                     int actn, int mods)
{
  s_rmww_instance->CreateZEPressureFromGLFW (wind, butt, actn, mods);

}

static std::unordered_map<int, char> s_glfw_lower_map
  {
    { GLFW_KEY_SPACE, ' '},
    { GLFW_KEY_APOSTROPHE, '\''},
    { GLFW_KEY_COMMA, ','},
    { GLFW_KEY_MINUS, '-'},
    { GLFW_KEY_PERIOD, '.'},
    { GLFW_KEY_SLASH, '/'},
    { GLFW_KEY_0, '0'},
    { GLFW_KEY_1, '1'},
    { GLFW_KEY_2, '2'},
    { GLFW_KEY_3, '3'},
    { GLFW_KEY_4, '4'},
    { GLFW_KEY_5, '5'},
    { GLFW_KEY_6, '6'},
    { GLFW_KEY_7, '7'},
    { GLFW_KEY_8, '8'},
    { GLFW_KEY_9, '9'},
    { GLFW_KEY_SEMICOLON, ';'},
    { GLFW_KEY_EQUAL, '='},
    { GLFW_KEY_A, 'a'},
    { GLFW_KEY_B, 'b'},
    { GLFW_KEY_C, 'c'},
    { GLFW_KEY_D, 'd'},
    { GLFW_KEY_E, 'e'},
    { GLFW_KEY_F, 'f'},
    { GLFW_KEY_G, 'g'},
    { GLFW_KEY_H, 'h'},
    { GLFW_KEY_I, 'i'},
    { GLFW_KEY_J, 'j'},
    { GLFW_KEY_K, 'k'},
    { GLFW_KEY_L, 'l'},
    { GLFW_KEY_M, 'm'},
    { GLFW_KEY_N, 'n'},
    { GLFW_KEY_O, 'o'},
    { GLFW_KEY_P, 'p'},
    { GLFW_KEY_Q, 'q'},
    { GLFW_KEY_R, 'r'},
    { GLFW_KEY_S, 's'},
    { GLFW_KEY_T, 't'},
    { GLFW_KEY_U, 'u'},
    { GLFW_KEY_V, 'v'},
    { GLFW_KEY_W, 'w'},
    { GLFW_KEY_X, 'x'},
    { GLFW_KEY_Y, 'y'},
    { GLFW_KEY_Z, 'z'},
    { GLFW_KEY_LEFT_BRACKET, '['},
    { GLFW_KEY_BACKSLASH, '\\'},
    { GLFW_KEY_RIGHT_BRACKET, ']'},
    { GLFW_KEY_GRAVE_ACCENT, '`'}
  };

static std::unordered_map<int, char> s_glfw_upper_map
  {
    { GLFW_KEY_SPACE, ' '},
    { GLFW_KEY_APOSTROPHE, '\"'},
    { GLFW_KEY_COMMA, '<'},
    { GLFW_KEY_MINUS, '_'},
    { GLFW_KEY_PERIOD, '>'},
    { GLFW_KEY_SLASH, '?'},
    { GLFW_KEY_0, ')'},
    { GLFW_KEY_1, '!'},
    { GLFW_KEY_2, '@'},
    { GLFW_KEY_3, '#'},
    { GLFW_KEY_4, '$'},
    { GLFW_KEY_5, '%'},
    { GLFW_KEY_6, '^'},
    { GLFW_KEY_7, '&'},
    { GLFW_KEY_8, '*'},
    { GLFW_KEY_9, '('},
    { GLFW_KEY_SEMICOLON, ':'},
    { GLFW_KEY_EQUAL, '+'},
    { GLFW_KEY_A, 'A'},
    { GLFW_KEY_B, 'B'},
    { GLFW_KEY_C, 'C'},
    { GLFW_KEY_D, 'D'},
    { GLFW_KEY_E, 'E'},
    { GLFW_KEY_F, 'F'},
    { GLFW_KEY_G, 'G'},
    { GLFW_KEY_H, 'H'},
    { GLFW_KEY_I, 'I'},
    { GLFW_KEY_J, 'J'},
    { GLFW_KEY_K, 'K'},
    { GLFW_KEY_L, 'L'},
    { GLFW_KEY_M, 'M'},
    { GLFW_KEY_N, 'N'},
    { GLFW_KEY_O, 'O'},
    { GLFW_KEY_P, 'P'},
    { GLFW_KEY_Q, 'Q'},
    { GLFW_KEY_R, 'R'},
    { GLFW_KEY_S, 'S'},
    { GLFW_KEY_T, 'T'},
    { GLFW_KEY_U, 'U'},
    { GLFW_KEY_V, 'V'},
    { GLFW_KEY_W, 'W'},
    { GLFW_KEY_X, 'X'},
    { GLFW_KEY_Y, 'Y'},
    { GLFW_KEY_Z, 'Z'},
    { GLFW_KEY_LEFT_BRACKET, '{'},
    { GLFW_KEY_BACKSLASH, '|'},
    { GLFW_KEY_RIGHT_BRACKET, '}'},
    { GLFW_KEY_GRAVE_ACCENT, '~'}
  };

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  (void)window;
  (void)scancode;

  char ch = ' ';
  const char *ch_ptr = &ch;
  if (mods & GLFW_MOD_SHIFT)
    {
      auto it = s_glfw_upper_map.find (key);
      if (it == s_glfw_upper_map.end ())
        return;

      ch = it->second;
    }
  else
    {
      auto it = s_glfw_lower_map.find (key);
      if (it == s_glfw_lower_map.end ())
        return;

      ch = it->second;
    }

  if (GLFW_PRESS == action)
    {
      ZEYowlAppearEvent yaevt (ch_ptr);
      s_rmww_instance->SprayYowlAppear(&yaevt);
    }
  else if (GLFW_REPEAT == action)
    {
      ZEYowlRepeatEvent yrevt (ch_ptr);
      s_rmww_instance->SprayYowlRepeat (&yrevt);
    }
  else if (GLFW_RELEASE == action)
    {
      ZEYowlVanishEvent yvevt (ch_ptr);
      s_rmww_instance->SprayYowlVanish (&yvevt);
    }
}

GLFWWaterWorks::GLFWWaterWorks (GLFWwindow *window)
  : m_temp_sprinkler {nullptr}
{
  s_rmww_instance = this;

  glfwSetKeyCallback (window, glfw_key_callback);
  glfwSetCursorPosCallback (window, glfw_mousepos_callback);
  glfwSetMouseButtonCallback (window, glfw_mousebutt_callback);
}

void GLFWWaterWorks::Drain (MultiSprinkler *_ms)
{
  m_temp_sprinkler = _ms;
  glfwPollEvents();
}

void GLFWWaterWorks::SpraySpatialMove (ZESpatialMoveEvent *_zsme)
{
  m_temp_sprinkler->Spray (_zsme);
}

void GLFWWaterWorks::SpraySpatialHarden (ZESpatialHardenEvent *_zshe)
{
  m_temp_sprinkler->Spray (_zshe);
}

void GLFWWaterWorks::SpraySpatialSoften (ZESpatialSoftenEvent *_zsse)
{
  m_temp_sprinkler->Spray (_zsse);
}

void GLFWWaterWorks::SprayYowlAppear (ZEYowlAppearEvent *_zyae)
{
  m_temp_sprinkler->Spray (_zyae);
}

void GLFWWaterWorks::SprayYowlRepeat (ZEYowlRepeatEvent *_zyre)
{
  m_temp_sprinkler->Spray (_zyre);
}

void GLFWWaterWorks::SprayYowlVanish (ZEYowlVanishEvent *_zyve)
{
  m_temp_sprinkler->Spray (_zyve);
}

static std::string const s_glfw_mouse_name = "mouse-0";

void GLFWWaterWorks::CreateZEMoveFromGLFW (GLFWwindow *_window, f64 _x, f64 _y)
{
  GraphicsApplication * const ga = GraphicsApplication::GetApplication();
  CMVTrefoil *leaf = nullptr;
  i32 const maes_count = ga->NumMaeses ();
  for (i32 q = 0;  q < maes_count;  ++q)
    {
      CMVTrefoil *l = ga->NthRenderLeaf (q);
      if (_x >= l->view.fb_pix_l  &&  _x < l->view.fb_pix_r
          && _y >= l->view.fb_pix_b  &&  _y < l->view.fb_pix_t)
        {
          leaf = l;
          break;
        }
    }

  if (! leaf)
    { fprintf (stderr,
               "in window %p: mouse at [%.1lf, %.1lf], but no Maes/bgfx-view\n",
               _window, _x, _y);
      return;
    }

  _x -= leaf->view.fb_pix_l;
  _y = leaf->view.fb_pix_t - _y - 1;   // freakin' (0,0) at the top left...
  f64 const x_normed = _x / (f64)(leaf->view.fb_pix_r - leaf->view.fb_pix_l);
  f64 const y_normed = _y / (f64)(leaf->view.fb_pix_t - leaf->view.fb_pix_b);
  Bolex *camera = leaf->cam;

  Vect thr = camera->ViewLoc ()  +  camera->ViewDist () * camera->ViewAim ();
  f64 wid = camera->IsPerspectiveTypeOthographic ()  ?  camera->ViewOrthoWid ()
    :  camera->ViewDist () * 2.0 * tan (0.5 * camera->ViewHorizAngle ());
  f64 hei = camera->IsPerspectiveTypeOthographic ()  ?  camera->ViewOrthoHei ()
    :  camera->ViewDist () * 2.0 * tan (0.5 * camera->ViewVertAngle ());
  Vect ovr = camera->ViewAim () . Cross (camera->ViewUp ()) . Norm ();
  Vect upp = ovr . Cross (camera->ViewAim ());

  thr += (x_normed - 0.5) * wid * ovr  +  (y_normed - 0.5) * hei * upp;
  Vect frm = camera->IsPerspectiveTypeProjection ()  ?  camera->ViewLoc ()
    :  thr - camera->ViewDist () * camera->ViewAim ();

  ZESpatialMoveEvent smev (s_glfw_mouse_name, frm, (thr - frm) . Norm (), ovr);

  m_mouse_state.valid = true;
  m_mouse_state.event.AdoptParticulars (smev);

  SpraySpatialMove(&smev);
}

void GLFWWaterWorks::CreateZEPressureFromGLFW (GLFWwindow *wind, int butt,
                                               int actn, int mods)
{
  (void)wind;
  (void)mods;
  u64 const which_button = 0x01 << butt;
  f64 const pressure = actn == GLFW_PRESS ? 1.0 : 0.0;

  u64 evlv_butt = m_mouse_state.valid  ?  m_mouse_state.button_state  :  0u;
  ZESpatialPressureEvent *spev = nullptr;
  if ((evlv_butt & which_button) == 0  &&  pressure > 0.0)
    { evlv_butt ^= which_button;
      ZESpatialHardenEvent *shev = m_mouse_state.valid
        ?  new ZESpatialHardenEvent (m_mouse_state.event, which_button, pressure)
        :  new ZESpatialHardenEvent (s_glfw_mouse_name, which_button, pressure);

      SpraySpatialHarden (shev);
      spev = shev;
    }
  else if ((evlv_butt & which_button) == which_button  &&  pressure < 1.0)
    { evlv_butt ^= which_button;
      ZESpatialSoftenEvent *ssev = m_mouse_state.valid
        ?  new ZESpatialSoftenEvent (m_mouse_state.event, which_button, pressure)
        :  new ZESpatialSoftenEvent (s_glfw_mouse_name, which_button, pressure);

      SpraySpatialSoften(ssev);
      spev = ssev;
    }
  else
    { // odd for sure: button was already in the 'new' state...
      return;
    }

  m_mouse_state.event.AdoptParticulars(*spev);
  m_mouse_state.button_state = evlv_butt;
}

}
