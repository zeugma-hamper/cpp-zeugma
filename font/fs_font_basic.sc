$input v_color0, v_texcoord0

#include <bgfx_shader.sh>
#include <common/common.sh>

uniform vec4 u_adj_color;

SAMPLERCUBE(s_texColor, 0);

layout(location = 0) out vec4 out_color;

void main()
{
	vec4 color = texture(s_texColor, v_texcoord0.xyz);
	int index = int(v_texcoord0.w*4.0 + 0.5);
	float alpha = index < 1 ? color.z :
		index < 2 ? color.y :
		index < 3 ? color.x : color.w;
	out_color = u_adj_color * vec4(v_color0.xyz, v_color0.a * alpha);
}
