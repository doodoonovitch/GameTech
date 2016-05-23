layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec3 outData;
layout(location = 2) out vec3 outNormal;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	vec3 color = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords).rgb;
	outData = WriteOutData(SKYBOX_RENDERER_ID , color, vec3(0), int(0), vec3(0));
	outPosition = fs_in.TexCoords;
	outNormal = vec3(0);
}
