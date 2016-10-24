//layout(location = 0) out vec3 outPosition;
layout(location = 0) out vec4 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

in VS_OUT
{
	vec4 Position;
	vec4 Color;
} fs_in;

void main(void)
{
	//outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(packHalf2x16(vec2(0))));
	outAlbedoAndStatus = uvec3(fs_in.Color.xyz * 255, AXIS_RENDERER_ID);
}
