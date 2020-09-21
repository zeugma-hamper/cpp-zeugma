$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D (u_video_texture, 0);
SAMPLER2D (u_video_matte, 1);

layout(location = 0) out vec4 out_color;

void main()
{
  out_color = vec4 (texture2D (u_video_texture, v_uv).rgb,
                    texture2D (u_video_matte, v_uv).r);
}