$input a_position, a_texcoord0
$output v_uv

#include <bgfx_shader.sh>

#include <over_up.sh>

uniform vec4 u_over;
uniform vec4 u_up;

// requires normalized orientation vectors
void main()
{
  vec4 norm = calculate_norm (u_over/wh.x, u_up/wh.y);
  mat4 rot = make_rot_matrix (u_over, u_up, norm);

  gl_Position =  = mul (u_modelViewProj, mul (rot, a_position));
  v_uv = a_texcoord0;
}
