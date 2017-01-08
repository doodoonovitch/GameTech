layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

layout (std430, binding = 2) buffer Materials
{
	ModelMaterial u_Materials[];
};

uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(uint samplerIndex, vec2 texUV, uint layerIndex);


in GS_OUT
{
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ModelDQ;
} fs_in;

void main(void)
{
	MaterialData mat;

	mat.mRendererId = MODEL_RENDERER_ID;

	mat.mBaseColor = vec3(u_Materials[fs_in.MaterialIndex].mBaseColorR, u_Materials[fs_in.MaterialIndex].mBaseColorG, u_Materials[fs_in.MaterialIndex].mBaseColorB);
	mat.mRoughness = u_Materials[fs_in.MaterialIndex].mRoughness;
	mat.mMetallic =  u_Materials[fs_in.MaterialIndex].mMetallic;
	mat.mPorosity =  u_Materials[fs_in.MaterialIndex].mPorosity;
	mat.mEnvMapType = u_Materials[fs_in.MaterialIndex].mEnvMapType;


	int baseColorTextureIndex = u_Materials[fs_in.MaterialIndex].mBaseColorTextureIndex;
	if (baseColorTextureIndex != -1)
	{
		int baseColorSamplerIndex = u_Materials[fs_in.MaterialIndex].mBaseColorSamplerIndex;
		mat.mBaseColor = mat.mBaseColor * TexGet(baseColorSamplerIndex, vec2(fs_in.TexUV), baseColorTextureIndex).xyz;
	}

	int metallicTextureIndex = u_Materials[fs_in.MaterialIndex].mMetallicTextureIndex;
	if(metallicTextureIndex != -1)
	{
		int metallicSamplerIndex = u_Materials[fs_in.MaterialIndex].mMetallicSamplerIndex;
		mat.mMetallic = mat.mMetallic * TexGet(metallicSamplerIndex, vec2(fs_in.TexUV), metallicTextureIndex).x;
	}

	int roughnessTextureIndex = u_Materials[fs_in.MaterialIndex].mRoughnessTextureIndex;
	if (roughnessTextureIndex != -1)
	{
		int roughnessSamplerIndex = u_Materials[fs_in.MaterialIndex].mRoughnessSamplerIndex;
		mat.mRoughness = mat.mRoughness * TexGet(roughnessSamplerIndex, vec2(fs_in.TexUV), roughnessTextureIndex).x;
	}

	int emissiveTextureIndex = u_Materials[fs_in.MaterialIndex].mEmissiveTextureIndex;
	if(emissiveTextureIndex != -1)
	{
		int emissiveSamplerIndex = u_Materials[fs_in.MaterialIndex].mEmissiveSamplerIndex;
		mat.mEmissive = u_Materials[fs_in.MaterialIndex].mEmissive * TexGet(emissiveSamplerIndex, vec2(fs_in.TexUV), emissiveTextureIndex).xyz;
	}
	else
	{
		mat.mEmissive =  mat.mBaseColor * u_Materials[fs_in.MaterialIndex].mEmissive;
	}

	vec3 normal;
	int normalTextureIndex = u_Materials[fs_in.MaterialIndex].mNormalTextureIndex;
	if ((normalTextureIndex != -1))
	{
		int normalSamplerIndex = u_Materials[fs_in.MaterialIndex].mNormalSamplerIndex;
		vec3 bumpMapNormal = TexGet(normalSamplerIndex, fs_in.TexUV, normalTextureIndex).xyz;
		bumpMapNormal = bumpMapNormal * 2.0 - vec3(1.0, 1.0, 1.0);

		normal = ComputeBumpedNormal(fs_in.Normal, fs_in.Tangent, bumpMapNormal);
		normal = dqTransformNormal(normal, fs_in.ModelDQ);
	}
	else
	{
		normal = dqTransformNormal(normalize(fs_in.Normal), fs_in.ModelDQ);
	}

	outNormal = normal;
	EncodeMaterialData(outUI32Buffer1, mat);

}
