layout(location = 0) out vec4 outNormalAndEmissive;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out uint outRoughnessAndOthers;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	vec3 color = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords).rgb;
	WriteOutData(outAlbedoAndStatus, outRoughnessAndOthers, SKYBOX_RENDERER_ID , mat.DiffuseColor, .0f, 0.f, 0.f);
	outNormalAndEmissive = vec4(0);
}
