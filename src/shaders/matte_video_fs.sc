$input v_uv

#include <bgfx_shader.sh>
#include <yuv.sh>

SAMPLER2D (u_video_texture0, 0);
SAMPLER2D (u_video_texture1, 1);
SAMPLER2D (u_video_texture2, 2);
SAMPLER2D (u_video_matte, 3);

uniform vec4 u_flags;
uniform vec4 u_mix_color;

layout (location = 0) out vec4 out_color;

#define ENABLE_MIX u_flags.y

//TODO: premultiplied alpha?
//TODO: gamma?
void main()
{
  float alpha = 1.0 - texture2D (u_video_matte, v_uv).r;
  if (alpha < 0.01 && ENABLE_MIX == 0.0)
    discard;

  vec3 yuv = vec3 (texture2D (u_video_texture0, v_uv).r,
                   texture2D (u_video_texture1, v_uv).r,
                   texture2D (u_video_texture2, v_uv).r);

  vec4 oc = vec4 (convert_bt601_scaled (yuv), alpha);
  if (ENABLE_MIX > 0.0 && alpha < 1.0)
    {
      oc = vec4 (yuv.r, yuv.r, yuv.r, 1.0) * u_mix_color;
    }

  out_color = oc;
}
