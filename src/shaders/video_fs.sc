$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D (u_video_texture, 0);

void main()
{
  gl_FragColor = texture2D (u_video_texture, v_uv);
}