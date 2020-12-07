$input v_color0, v_texcoord0

#include <bgfx_shader.sh>
#include <common/common.sh>

uniform vec4 u_adj_color;

SAMPLERCUBE(s_texColor, 0);

layout(location = 0) out vec4 out_color;

void main()
{
	int index = int(v_texcoord0.w*4.0 + 0.5);
	vec3 dx3 = dFdx(v_texcoord0.xyz);
	vec3 dy3 = dFdy(v_texcoord0.xyz);
	vec3 decal = 0.166667 * dx3;
	vec3 sampleLeft = v_texcoord0.xyz - decal;
	vec3 sampleRight = v_texcoord0.xyz + decal;

	float left_dist  = texture(s_texColor, sampleLeft).zyxw[index];
	float right_dist = texture(s_texColor, sampleRight).zyxw[index];

	float dist = 0.5 * (left_dist + right_dist);

	float dx = length(dx3);
	float dy = length(dy3);
	float w = 16.0*0.5*(dx+dy);

	vec3 sub_color = smoothstep(0.5 - w, 0.5 + w, vec3(left_dist, dist, right_dist));
	out_color.xyz = sub_color*v_color0.w;
	out_color.w = dist*v_color0.w;
  out_color = u_adj_color * out_color;
}
