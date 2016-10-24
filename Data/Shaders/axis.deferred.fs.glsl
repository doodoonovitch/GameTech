layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

in VS_OUT
{
	vec4 Color;
} fs_in;

void main(void)
{
	outAlbedoAndStatus = uvec3(fs_in.Color.xyz * 255, AXIS_RENDERER_ID);
}
