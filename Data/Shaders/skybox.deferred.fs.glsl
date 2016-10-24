layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	vec3 color = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords).rgb;
	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, SKYBOX_RENDERER_ID , vec3(0), color, 0, vec3(0));
	outNormal = vec3(0);
}
