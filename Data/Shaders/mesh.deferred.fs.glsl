layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

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
	flat DualQuat ViewModelDQ;
} fs_in;

void main(void)
{
	
	vec3 materialDiffuse = vec3(u_Materials[fs_in.MaterialIndex].mDiffuseR, u_Materials[fs_in.MaterialIndex].mDiffuseG, u_Materials[fs_in.MaterialIndex].mDiffuseB);
	vec3 materialSpecular = vec3(u_Materials[fs_in.MaterialIndex].mSpecularR, u_Materials[fs_in.MaterialIndex].mSpecularG, u_Materials[fs_in.MaterialIndex].mSpecularB);
	vec3 materialEmissive = vec3(u_Materials[fs_in.MaterialIndex].mEmissiveR, u_Materials[fs_in.MaterialIndex].mEmissiveG, u_Materials[fs_in.MaterialIndex].mEmissiveB);
	float roughness = u_Materials[fs_in.MaterialIndex].mRoughness;

	int diffuseTextureIndex = u_Materials[fs_in.MaterialIndex].mDiffuseTextureIndex;
	if (diffuseTextureIndex != -1)
	{
		int diffuseSamplerIndex = u_Materials[fs_in.MaterialIndex].mDiffuseSamplerIndex;
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec2(fs_in.TexUV), diffuseTextureIndex).xyz;
	}

	int specularTextureIndex = u_Materials[fs_in.MaterialIndex].mSpecularTextureIndex;
	if(specularTextureIndex != -1)
	{
		int specularSamplerIndex = u_Materials[fs_in.MaterialIndex].mSpecularSamplerIndex;
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec2(fs_in.TexUV), specularTextureIndex).xyz;
	}

	int emissiveTextureIndex = u_Materials[fs_in.MaterialIndex].mEmissiveTextureIndex;
	if (emissiveTextureIndex != -1)
	{
		int emissiveSamplerIndex = u_Materials[fs_in.MaterialIndex].mEmissiveSamplerIndex;
		materialEmissive = materialEmissive * TexGet(emissiveSamplerIndex, vec2(fs_in.TexUV), emissiveTextureIndex).xyz;
	}

	int roughnessTextureIndex = u_Materials[fs_in.MaterialIndex].mRoughnessTextureIndex;
	if (roughnessTextureIndex != -1)
	{
		int roughnessSamplerIndex = u_Materials[fs_in.MaterialIndex].mRoughnessSamplerIndex;
		roughness = roughness * TexGet(roughnessSamplerIndex, vec2(fs_in.TexUV), roughnessTextureIndex).x;
	}

	vec3 normal;
	int normalTextureIndex = u_Materials[fs_in.MaterialIndex].mNormalTextureIndex;
	if ((normalTextureIndex != -1))
	{
		int normalSamplerIndex = u_Materials[fs_in.MaterialIndex].mNormalSamplerIndex;
		vec3 bumpMapNormal = TexGet(normalSamplerIndex, fs_in.TexUV, normalTextureIndex).xyz;
		bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

		normal = ComputeBumpedNormal(fs_in.Normal, fs_in.Tangent, bumpMapNormal);
		normal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	}
	else
	{
		normal = dqTransformNormal(normalize(fs_in.Normal), fs_in.ViewModelDQ);
	}

	outNormal = vec3(normal.xyz);

	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, CUBE_RENDERER_ID , materialDiffuse, materialSpecular, roughness, materialEmissive);

}
