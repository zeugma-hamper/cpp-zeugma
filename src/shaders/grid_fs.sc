
$input wrld_p


#include <bgfx_shader.sh>


uniform vec4 u_color;
uniform vec4 u_warp;
uniform vec4 u_weft;


layout(location = 0) out vec4 out_color;


void main()
{ float tx = mod (dot (wrld_p, u_warp) - 0.5, 1.0);
  float ty = mod (dot (wrld_p, u_weft) - 0.5, 1.0);;
  out_color = max (smoothstep (0.35, 0.50, tx)  -  smoothstep (0.50, 0.65, tx),
                   smoothstep (0.35, 0.50, ty)  -  smoothstep (0.50, 0.65, ty))
    * u_color;
}
