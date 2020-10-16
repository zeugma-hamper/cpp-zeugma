$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D (u_video_texture0, 0);
SAMPLER2D (u_video_texture1, 1);
SAMPLER2D (u_video_texture2, 2);
SAMPLER2D (u_video_matte, 3);

layout (location = 0) out vec4 out_color;

//TODO: premultiplied alpha?
//TODO: gamma?
void main()
{
  vec4 vd = texture2D (u_video_texture0, v_uv);
  float alpha = 1.0 - texture2D (u_video_matte, v_uv).r;
  if (alpha < 0.01)
    discard;

  out_color = vec4 (vd.rgb, alpha);
}
