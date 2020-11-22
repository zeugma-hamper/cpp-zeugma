
$input wrld_p


#include <bgfx_shader.sh>


uniform vec4 u_color;
uniform vec4 u_warp;
uniform vec4 u_weft;


layout(location = 0) out vec4 out_color;


void main()
{ float tx = mod (dot (wrld_p, u_warp), 1.0);
  float ty = mod (dot (wrld_p, u_weft), 1.0);;
  out_color = tx * ty * u_color;
}
