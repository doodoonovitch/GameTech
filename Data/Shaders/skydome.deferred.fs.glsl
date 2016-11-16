layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

in VS_OUT
{
	vec3 Color;
} fs_in;

void main(void)
{
	MaterialData mat;

	mat.mBaseColor = vec3(16/255, 232/255, 255/255);
	mat.mMetallic = 0;
	mat.mRoughness = 0;
	mat.mPorosity = 0;
	mat.mEmissive = fs_in.Color;
	mat.mRendererId = SKYBOX_RENDERER_ID;

	EncodeMaterialData(outUI32Buffer1, mat);
}
