layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

uniform samplerBuffer u_materialDataSampler;
uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(int samplerIndex, vec3 p);
vec4 TexGetNormal(int samplerIndex, vec3 p);


in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ViewModelDQ;
} fs_in;

void main(void)
{
	vec4 matData;

	matData = texelFetch(u_materialDataSampler, fs_in.MaterialIndex);
	vec3 materialDiffuse = matData.xyz;
	uint bitfieldValue = floatBitsToUint(matData.w);
	int diffuseTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int diffuseSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	int specularTextureIndex = int(bitfieldValue & uint(255));
	int specularSamplerIndex = int((bitfieldValue >> 8) & uint(255));

	matData = texelFetch(u_materialDataSampler, fs_in.MaterialIndex + 1);
	vec3 materialSpecular = matData.xyz;
	bitfieldValue = floatBitsToUint(matData.w);
	float roughness = float((bitfieldValue & uint(32767)) / 32767.f);
	int roughnessTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int roughnessSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	
	matData = texelFetch(u_materialDataSampler, fs_in.MaterialIndex + 2);
	vec3 materialEmissive = matData.xyz;
	bitfieldValue = floatBitsToUint(matData.w);
	int emissiveTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int emissiveSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	int normalTextureIndex = int(bitfieldValue & uint(255));
	int normalSamplerIndex = int((bitfieldValue >> 8) & uint(255));
	/*
	if (diffuseTextureIndex != -1 && diffuseSamplerIndex != -1)
	{
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec3(fs_in.TexUV, diffuseTextureIndex)).xyz;
	}

	if (specularTextureIndex != -1 && specularSamplerIndex != -1)
	{
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec3(fs_in.TexUV, specularTextureIndex)).xyz;
	}

	if (emissiveTextureIndex != -1 && emissiveSamplerIndex != -1)
	{
		materialEmissive = materialEmissive * TexGet(emissiveSamplerIndex, vec3(fs_in.TexUV, emissiveTextureIndex)).xyz;
	}

	if (roughnessTextureIndex != -1 && roughnessSamplerIndex != -1)
	{
		roughness = roughness * TexGet(roughnessSamplerIndex, vec3(fs_in.TexUV, roughnessTextureIndex)).x;
	}
	*/
	vec3 normal;
	if (normalTextureIndex != -1 && normalSamplerIndex != -1)
	{
		vec3 bumpMapNormal = TexGet(normalSamplerIndex, vec3(fs_in.TexUV, normalTextureIndex)).xyz;
		bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

		normal = ComputeBumpedNormal(fs_in.Normal, fs_in.Tangent, bumpMapNormal);
		normal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	}
	else
	{
		normal = dqTransformNormal(normalize(fs_in.Normal), fs_in.ViewModelDQ);
	}
	
	outPosition = fs_in.Position.xyz;
	outNormal = normal.xyz;

	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, CUBE_RENDERER_ID , materialDiffuse, materialSpecular, roughness, materialEmissive);

}
