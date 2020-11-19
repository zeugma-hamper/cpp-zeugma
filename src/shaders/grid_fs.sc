
#include <bgfx_shader.sh>


uniform vec4 u_color;
uniform vec4 u_warp;
uniform vec4 u_weft;


layout(location = 0) out vec4 out_color;


void main()
{
  out_color = u_color;
}
