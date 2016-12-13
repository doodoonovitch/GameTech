layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_textureSampler;

#ifdef PER_PIXEL_NORMAL

uniform sampler2D u_NormalMapSampler;
uniform float u_MaxAmplitude;

in TES_OUT
{
	vec2 TexUV;
	vec3 Position;
	flat int MapIndex;
} fs_in;


#else


in TES_OUT
{
	vec2 TexUV;
	vec3 Position;
	vec3 Normal;
	flat int MapIndex;
} fs_in;

#endif

struct Material
{
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Roughness;
};



void main()
{
	vec2 texUV = fs_in.TexUV;

#ifdef PER_PIXEL_NORMAL
	float nX = textureOffset(u_NormalMapSampler, texUV, ivec2(-1, 0)).r - textureOffset(u_NormalMapSampler, texUV, ivec2(1, 0)).r;
	float nZ = textureOffset(u_NormalMapSampler, texUV, ivec2(0, -1)).r - textureOffset(u_NormalMapSampler, texUV, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX, 2, nZ));
#else
	vec3 normal = normalize(fs_in.Normal);
#endif

	MaterialData mat;

	mat.mRendererId = MODEL_RENDERER_ID;

	mat.mBaseColor = vec3(0.2);
	mat.mRoughness = .2;
	mat.mMetallic =  0;
	mat.mPorosity =  0;
	mat.mEmissive =  vec3(0);

	//vec3 refractColor = texture(u_textureSampler, texUV).rgb;
	//mat.DiffuseColor = refractColor;


	outNormal = normal;
	EncodeMaterialData(outUI32Buffer1, mat);
}
