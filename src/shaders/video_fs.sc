$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D (u_video_texture, 0);

layout(location = 0) out vec4 out_color;

void main()
{
  out_color = texture2D (u_video_texture, v_uv);
}