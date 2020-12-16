
$input a_position

$output wrld_p


#include <bgfx_shader.sh>


void main()
{ wrld_p = mul (u_model[0], vec4 (a_position, 1.0));
  gl_Position = mul (u_modelViewProj, vec4 (a_position, 1.0));
}
