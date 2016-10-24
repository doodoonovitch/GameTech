layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

in VS_OUT
{
	vec3 Color;
} fs_in;

void main(void)
{
	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, SKYBOX_RENDERER_ID , vec3(0), fs_in.Color, 0, vec3(0));
	outNormal = vec3(0);
}
