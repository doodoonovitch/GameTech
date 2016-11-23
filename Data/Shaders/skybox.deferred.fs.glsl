layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

uniform samplerCube u_SkyboxCubeMapSampler;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;

void main(void)
{
	MaterialData mat;

	mat.mBaseColor = vec3(0);
	mat.mMetallic = 0;
	mat.mRoughness = 0;
	mat.mPorosity = 0;
	mat.mEmissive = texture(u_SkyboxCubeMapSampler, fs_in.TexCoords).rgb;
	mat.mRendererId = SKYBOX_RENDERER_ID;

	EncodeMaterialData(outUI32Buffer1, mat);
}
