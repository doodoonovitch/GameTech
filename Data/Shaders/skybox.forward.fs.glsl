layout(location = 0) out vec4 outColor;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	outColor = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords);
}
