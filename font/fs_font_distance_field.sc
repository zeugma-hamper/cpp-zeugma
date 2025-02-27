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
	float rgba[4];
	rgba[0] = color.z;
	rgba[1] = color.y;
	rgba[2] = color.x;
	rgba[3] = color.w;
	float dist = rgba[index];

	float dx = length(dFdx(v_texcoord0.xyz) );
	float dy = length(dFdy(v_texcoord0.xyz) );
	float w = 16.0*0.5*(dx+dy);

	float alpha = smoothstep(0.5-w, 0.5+w, dist);
	out_color = u_adj_color * vec4(v_color0.xyz, v_color0.w*alpha);
}
