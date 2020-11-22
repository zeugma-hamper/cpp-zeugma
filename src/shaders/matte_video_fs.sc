
$input v_uv

#include <bgfx_shader.sh>
#include <yuv.sh>

uniform vec4 u_adj_color;

SAMPLER2D (u_video_texture0, 0);
SAMPLER2D (u_video_texture1, 1);
SAMPLER2D (u_video_texture2, 2);
SAMPLER2D (u_video_matte, 3);

layout (location = 0) out vec4 out_color;

//TODO: premultiplied alpha?
//TODO: gamma?
void main()
{
  float alpha = 1.0 - texture2D (u_video_matte, v_uv).r;
  if (alpha < 0.01)
    discard;

  vec3 yuv = vec3 (texture2D (u_video_texture0, v_uv).r,
                   texture2D (u_video_texture1, v_uv).r,
                   texture2D (u_video_texture2, v_uv).r);

  out_color = u_adj_color * vec4 (convert_bt601_scaled (yuv), alpha);
}
