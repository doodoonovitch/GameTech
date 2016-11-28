layout(location = 0) out vec4 outColor;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	outColor = vec4(texture(u_SkyboxCubeMapSampler, fs_in.TexCoords).xyz, 1);
	//outColor = vec4(0, 0, 1, 1);
}
