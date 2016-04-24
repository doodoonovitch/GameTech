layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec3 outData;
layout(location = 2) out vec3 outNormal;

in VS_OUT
{
	vec4 Position;
	vec2 TexUV;
} fs_in;


void main(void)
{
	outPosition = fs_in.Position.xyz;
	outNormal = vec3(0);
	outData = uvec3(packUnorm4x8(vec4(fs_in.TexUV.xy, 0, GRID_RENDERER_ID / 255)), 0, 0);
}
