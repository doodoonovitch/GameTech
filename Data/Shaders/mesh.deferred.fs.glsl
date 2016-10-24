layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

uniform samplerBuffer u_MaterialDataSampler;
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
	vec4 matData;

	matData = texelFetch(u_MaterialDataSampler, fs_in.MaterialIndex);
	vec3 materialDiffuse = matData.xyz;
	uint bitfieldValue = floatBitsToUint(matData.w);
	uint diffuseTextureIndex = ((bitfieldValue >> 16) & uint(255));
	uint diffuseSamplerIndex = ((bitfieldValue >> 24) & uint(255));
	uint specularTextureIndex = (bitfieldValue & uint(255));
	uint specularSamplerIndex = ((bitfieldValue >> 8) & uint(255));

	matData = texelFetch(u_MaterialDataSampler, fs_in.MaterialIndex + 1);
	vec3 materialSpecular = matData.xyz;
	bitfieldValue = floatBitsToUint(matData.w);
	float roughness = float((bitfieldValue & uint(32767)) / 32767.f);
	uint roughnessTextureIndex = ((bitfieldValue >> 16) & uint(255));
	uint roughnessSamplerIndex = ((bitfieldValue >> 24) & uint(255));
	
	matData = texelFetch(u_MaterialDataSampler, fs_in.MaterialIndex + 2);
	vec3 materialEmissive = matData.xyz;
	bitfieldValue = floatBitsToUint(matData.w);
	uint emissiveTextureIndex = ((bitfieldValue >> 16) & uint(255));
	uint emissiveSamplerIndex = ((bitfieldValue >> 24) & uint(255));
	uint normalTextureIndex = (bitfieldValue & uint(255));
	uint normalSamplerIndex = ((bitfieldValue >> 8) & uint(255));
	
	if ((diffuseTextureIndex != 255) && (diffuseSamplerIndex != 255))
	{
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec2(fs_in.TexUV), diffuseTextureIndex).xyz;
	}

	if ((specularTextureIndex != 255) && (specularSamplerIndex != 255))
	{
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec2(fs_in.TexUV), specularTextureIndex).xyz;
	}

	if ((emissiveTextureIndex != 255) && (emissiveSamplerIndex != 255))
	{
		materialEmissive = materialEmissive * TexGet(emissiveSamplerIndex, vec2(fs_in.TexUV), emissiveTextureIndex).xyz;
	}

	if ((roughnessTextureIndex != 255) && (roughnessSamplerIndex != 255))
	{
		roughness = roughness * TexGet(roughnessSamplerIndex, vec2(fs_in.TexUV), roughnessTextureIndex).x;
	}
	
	vec3 normal;
	if ((normalTextureIndex != 255) && (normalSamplerIndex != 255))
	{
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
