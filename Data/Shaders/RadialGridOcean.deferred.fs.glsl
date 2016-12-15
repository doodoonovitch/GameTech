layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_TextureSampler;


uniform int u_Frame0;
uniform sampler2DArray u_HeightMapsSampler;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


void main()
{
	vec3 texUV = vec3(fs_in.TexUV, u_Frame0);

	float nX = textureOffset(u_HeightMapsSampler, texUV, ivec2(-1, 0)).r - textureOffset(u_HeightMapsSampler, texUV, ivec2(1, 0)).r;
	float nZ = textureOffset(u_HeightMapsSampler, texUV, ivec2(0, -1)).r - textureOffset(u_HeightMapsSampler, texUV, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX, 2, nZ));

	MaterialData mat;

	mat.mRendererId = MODEL_RENDERER_ID;

	mat.mBaseColor = vec3(7.0 / 255.0, 136.0 / 255.0, 143.0 / 255.0);
	mat.mRoughness = .1;
	mat.mMetallic =  1;
	mat.mPorosity =  0;
	mat.mEmissive =  vec3(0);

	outNormal = normal;
	EncodeMaterialData(outUI32Buffer1, mat);
}
