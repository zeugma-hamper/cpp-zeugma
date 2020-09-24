$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(u_video_texture, 0);
SAMPLER2D(u_video_matte, 1);

layout (location = 0) out vec4 out_color;

void main()
{
  vec4 vd = texture2D (u_video_texture, v_uv);
  float alpha = 1.0 - texture2D (u_video_matte, v_uv).r;
  if (alpha < 0.01)
    discard;

  out_color = vec4 (vd.rgb, alpha);
}
