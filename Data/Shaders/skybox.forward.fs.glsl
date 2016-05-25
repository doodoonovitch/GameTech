layout(location = 0) out vec4 vFragColor;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	vFragColor = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords);
}
