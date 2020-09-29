$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D (u_video_texture0, 0);
SAMPLER2D (u_video_texture1, 1);
SAMPLER2D (u_video_texture2, 2);

layout(location = 0) out vec4 out_color;

void main()
{
  out_color = texture2D (u_video_texture0, v_uv);
}