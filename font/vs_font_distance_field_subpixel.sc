$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include "../bgfx/examples/common/common.sh"

uniform vec4 u_dimensions;
uniform vec4 u_over;
uniform vec4 u_up;

void main()
{
  vec4 norm = vec4 (cross (u_over.xyz, u_up.xyz), 0.0);
  mat4 rot = mat4 (u_over, u_up, norm, vec4 (0.0, 0.0, 0.0, 1.0));

  vec2 apos = vec2 (a_position.x, u_dimensions.y - a_position.y);
  float mx = max (u_dimensions.x, u_dimensions.y);
  vec2 bias = (u_dimensions.xy / mx) * (vec2 (-0.5, -0.5) - u_dimensions.zw);
  vec2 pos = (apos / mx) + bias;

	gl_Position = mul(u_modelViewProj, mul (rot, vec4(pos, 0.0, 1.0)));
	v_texcoord0 = a_texcoord0;
	v_color0 = a_color0;
}
