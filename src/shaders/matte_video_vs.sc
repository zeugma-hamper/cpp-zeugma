$output v_uv

#include <bgfx_shader.sh>

uniform vec4 u_dimensions;
uniform vec4 u_matte_dimensions;

void main()
{
  const vec4 positions[4] = vec4[4] (vec4 (-0.5,  0.5, 0.0, 1.0),
                                     vec4 (-0.5, -0.5, 0.0, 1.0),
                                     vec4 ( 0.5,  0.5, 0.0, 1.0),
                                     vec4 ( 0.5, -0.5, 0.0, 1.0));

  const vec2 uvs[4] = vec2[4] (vec2 (0.0, 0.0),
                               vec2 (0.0, 1.0),
                               vec2 (1.0, 0.0),
                               vec2 (1.0, 1.0));

  vec2 min = u_matte_dimensions.xy / u_dimensions.xy;
  vec2 max = u_matte_dimensions.zw / u_dimensions.xy;
  vec2 uv = min + ((max - min) * uvs[gl_VertexID]);
  vec2 wh = u_matte_dimensions.zw - u_matte_dimensions.xy;
  float inv_aspect = wh.y / wh.x;

  vec4 aspect_ratio = vec4 (1.0, inv_aspect, 1.0, 1.0);

  gl_Position = mul (u_modelViewProj, positions[gl_VertexID] * aspect_ratio);
  v_uv = uv;
}
