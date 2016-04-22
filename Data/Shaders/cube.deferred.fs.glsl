layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

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
	float materialShininess = float((bitfieldValue >> 16) & uint(65535));

	matData = texelFetch(u_materialDataSampler, fs_in.MaterialIndex + 2);
	vec3 materialEmissive = matData.xyz;
	bitfieldValue = floatBitsToUint(matData.w);
	int emissiveTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int emissiveSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	int normalTextureIndex = int(bitfieldValue & uint(255));
	int normalSamplerIndex = int((bitfieldValue >> 8) & uint(255));

	if (diffuseTextureIndex != -1)
	{
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec3(fs_in.TexUV, diffuseTextureIndex)).xyz;
	}

	if (specularTextureIndex != -1)
	{
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec3(fs_in.TexUV, specularTextureIndex)).xyz;
	}

	if (emissiveTextureIndex != -1)
	{
		materialEmissive = materialEmissive * TexGet(emissiveSamplerIndex, vec3(fs_in.TexUV, emissiveTextureIndex)).xyz;
	}

	vec3 bumpMapNormal = TexGet(normalSamplerIndex, vec3(fs_in.TexUV, normalTextureIndex)).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

	vec3 normal = ComputeBumpedNormal(fs_in.Normal, fs_in.Tangent, bumpMapNormal);
	normal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(packHalf2x16(-normal.xy/normal.z)));

	outData = uvec3(packUnorm4x8(vec4(materialDiffuse, CUBE_RENDERER_ID / 255)), packUnorm4x8(vec4(materialSpecular, materialShininess / 255)), packUnorm4x8(vec4(materialEmissive, 0)));
}
