layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

struct TerrainPerInstanceData
{
	ivec4 mMatIndex;
	vec3 mOrigin;
};

layout (std430, binding = 0) buffer PerInstanceData
{
	TerrainPerInstanceData u_PerInstanceData[];
};

layout (std430, binding = 1) buffer Materials
{
	ModelMaterial u_Materials[];
};

uniform vec3 u_Scale;
uniform float u_TriplanarBlendSharpness = 2.0;
uniform float u_TexScale = 0.1;

uniform sampler2DArray u_HeightMap;
uniform sampler2DArray u_NormalMap;
uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];
uniform sampler2DArray u_BlendMap;

in TES_OUT
{
	vec4 Blend;
	vec3 Position;
	vec2 TexUV;
	//float MipmapLevel;
	flat int MapIndex;
} fs_in;

struct Material
{
	vec3 BaseColor;
	float Metallic;
	float Roughness;
	float Height;
};

const float BlendEpsilon = 1.0f / 256.f;

vec4 TexGet(uint samplerIndex, vec2 texUV, uint layerIndex);

// Project the surface gradient (dhdx, dhdy) onto the surface (n, dpdx, dpdy)
vec3 CalculateSurfaceGradient(vec3 n, vec3 dpdx, vec3 dpdy, float dhdx, float dhdy)
{
    vec3 r1 = cross(dpdy, n);
    vec3 r2 = cross(n, dpdx);

    return (r1 * dhdx + r2 * dhdy) / dot(dpdx, r1);
}
 
// Calculate the surface normal using screen-space partial derivatives of the height field
vec3 PerturbNormal(vec3 position, vec3 normal, float height)
{
    vec3 dpdx = dFdx(position);
    vec3 dpdy = dFdy(position);
 
    float dhdx = dFdx(height);
    float dhdy = dFdy(height);
 
	return normalize(normal - CalculateSurfaceGradient(normal, dpdx, dpdy, dhdx, dhdy));
}

vec3 GetTriplanarWeights(vec3 normal)
{
	vec3 axisWeights;
	//axisWeights = (abs(normal) - vec3(0.2)) * 7;
	//axisWeights = pow(axisWeights, vec3(3));
	axisWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	axisWeights = axisWeights / (axisWeights.x + axisWeights.y + axisWeights.z);
	return axisWeights;
}

vec4 GetTexture(vec2 uvAxisX, vec2 uvAxisY, vec2 uvAxisZ, vec3 weights, uint samplerIndex, uint layerIndex)
{
	vec4 valX = TexGet(samplerIndex, uvAxisX, layerIndex) * weights.x;
	vec4 valY = TexGet(samplerIndex, uvAxisY, layerIndex) * weights.y;
	vec4 valZ = TexGet(samplerIndex, uvAxisZ, layerIndex) * weights.z;

	return valX + valY + valZ;
}


void GetMaterials1(inout MaterialData matData, inout float height, vec3 triplanarWeights, vec2 uvAxisX, vec2 uvAxisY, vec2 uvAxisZ, ivec4 matIndices, vec4 blends)
{
	height = 0;
	for(int i = 0; i < 4; ++i)
	{
		float blend = blends[i];
		int matIndex = matIndices[i];

		vec3 c = vec3(u_Materials[matIndex].mBaseColorR, u_Materials[matIndex].mBaseColorG, u_Materials[matIndex].mBaseColorB);
		int baseColorTextureIndex = u_Materials[matIndex].mBaseColorTextureIndex;
		if (baseColorTextureIndex != -1)
		{
			int baseColorSamplerIndex = u_Materials[matIndex].mBaseColorSamplerIndex;
			matData.mBaseColor += c * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, baseColorSamplerIndex, baseColorTextureIndex).xyz * blend;
		}
		else
		{
			matData.mBaseColor += c * blend;
		}

		int metallicTextureIndex = u_Materials[matIndex].mMetallicTextureIndex;
		if(metallicTextureIndex != -1)
		{
			int metallicSamplerIndex = u_Materials[matIndex].mMetallicSamplerIndex;
			matData.mMetallic += u_Materials[matIndex].mMetallic * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, metallicSamplerIndex, metallicTextureIndex).x * blend;
		}
		else
		{
			matData.mMetallic += u_Materials[matIndex].mMetallic * blend;
		}

		int roughnessTextureIndex = u_Materials[matIndex].mRoughnessTextureIndex;
		if (roughnessTextureIndex != -1)
		{
			int roughnessSamplerIndex = u_Materials[matIndex].mRoughnessSamplerIndex;
			matData.mRoughness += u_Materials[matIndex].mRoughness * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, roughnessSamplerIndex, roughnessTextureIndex).x * blend;
		}
		else
		{
			matData.mRoughness += u_Materials[matIndex].mRoughness * blend;
		}
		
		int emissiveTextureIndex = u_Materials[matIndex].mEmissiveTextureIndex;
		if(emissiveTextureIndex != -1)
		{
			int emissiveSamplerIndex = u_Materials[matIndex].mEmissiveSamplerIndex;
			matData.mEmissive += u_Materials[matIndex].mEmissive * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, emissiveSamplerIndex, emissiveTextureIndex).xyz * blend;
		}
		else
		{
			matData.mEmissive +=  matData.mBaseColor * u_Materials[matIndex].mEmissive * blend;
		}

		float h;
		int heightTextureIndex = u_Materials[matIndex].mHeightTextureIndex;
		if ((heightTextureIndex != -1) && (blend > BlendEpsilon))
		{
			int heightSamplerIndex = u_Materials[matIndex].mHeightSamplerIndex;
			h = (u_Materials[matIndex].mHeightOffset + u_Materials[matIndex].mHeightScale * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, heightSamplerIndex, heightTextureIndex).x) * blend;
		}
		else
		{
			h = (u_Materials[matIndex].mHeightOffset + u_Materials[matIndex].mHeightScale) * blend;
		}

		height += h;
	}
}

void GetMaterials2(inout MaterialData matData, inout float height, vec3 triplanarWeights, vec2 uvAxisX, vec2 uvAxisY, vec2 uvAxisZ, ivec4 matIndices, vec4 blends)
{
	height = -1;
	int index = -1;
	
	for(int i = 0; i < 4; ++i)
	{
		float h;
		float blend = blends[i];
		int matIndex = matIndices[i];

		int heightTextureIndex = u_Materials[matIndex].mHeightTextureIndex;
		if ((heightTextureIndex != -1) && (blend > BlendEpsilon))
		{
			int heightSamplerIndex = u_Materials[matIndex].mHeightSamplerIndex;
			h = (u_Materials[matIndex].mHeightOffset + u_Materials[matIndex].mHeightScale * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, heightSamplerIndex, heightTextureIndex).x) * blend;
		}
		else
		{
			h = (u_Materials[matIndex].mHeightOffset + u_Materials[matIndex].mHeightScale) * blend;
		}

		if(h > height)
		{
			index = i;
			height = h;
		}
	}

	if(index >= 0)
	{
		int matIndex = matIndices[index];

		matData.mBaseColor = vec3(u_Materials[matIndex].mBaseColorR, u_Materials[matIndex].mBaseColorG, u_Materials[matIndex].mBaseColorB);
		matData.mRoughness = u_Materials[matIndex].mRoughness;
		matData.mMetallic =  u_Materials[matIndex].mMetallic;
		matData.mPorosity =  u_Materials[matIndex].mPorosity;

		int baseColorTextureIndex = u_Materials[matIndex].mBaseColorTextureIndex;
		if (baseColorTextureIndex != -1)
		{
			int baseColorSamplerIndex = u_Materials[matIndex].mBaseColorSamplerIndex;
			matData.mBaseColor = matData.mBaseColor * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, baseColorSamplerIndex, baseColorTextureIndex).xyz;
		}

		int metallicTextureIndex = u_Materials[matIndex].mMetallicTextureIndex;
		if(metallicTextureIndex != -1)
		{
			int metallicSamplerIndex = u_Materials[matIndex].mMetallicSamplerIndex;
			matData.mMetallic = matData.mMetallic * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, metallicSamplerIndex, metallicTextureIndex).x;
		}

		int roughnessTextureIndex = u_Materials[matIndex].mRoughnessTextureIndex;
		if (roughnessTextureIndex != -1)
		{
			int roughnessSamplerIndex = u_Materials[matIndex].mRoughnessSamplerIndex;
			matData.mRoughness = matData.mRoughness * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, roughnessSamplerIndex, roughnessTextureIndex).x;
		}

		int emissiveTextureIndex = u_Materials[matIndex].mEmissiveTextureIndex;
		if(emissiveTextureIndex != -1)
		{
			int emissiveSamplerIndex = u_Materials[matIndex].mEmissiveSamplerIndex;
			matData.mEmissive = u_Materials[matIndex].mEmissive * GetTexture(uvAxisX, uvAxisY, uvAxisZ, triplanarWeights, emissiveSamplerIndex, emissiveTextureIndex).xyz;
		}
		else
		{
			matData.mEmissive =  matData.mBaseColor * u_Materials[matIndex].mEmissive;
		}
	}
	else
	{
		matData.mBaseColor = vec3(0, 0, 0);
		matData.mRoughness = 1;
		matData.mMetallic = 0;
		matData.mPorosity = 0;
		height = 0;
	}
}

void main()
{
	MaterialData matData;
	matData.mRendererId = /*DEBUG_RENDERER_ID; */ MODEL_RENDERER_ID;
	matData.mBaseColor = vec3(0);
	matData.mRoughness = 0;
	matData.mMetallic = 0;
	matData.mEmissive = vec3(0);

	vec3 texCoord = vec3(fs_in.TexUV, fs_in.MapIndex);
	vec3 normal = normalize(texture(u_NormalMap, texCoord).xyz);
	//float nX = textureOffset(u_HeightMap, texCoord, ivec2(-1, 0)).r - textureOffset(u_HeightMap, texCoord, ivec2(1, 0)).r;
	//float nZ = textureOffset(u_HeightMap, texCoord, ivec2(0, -1)).r - textureOffset(u_HeightMap, texCoord, ivec2(0, 1)).r;
	//vec3 normal = normalize(vec3(nX * u_Scale.y, (u_Scale.x + u_Scale.z), nZ * u_Scale.y));


	//float d = distance(fs_in.Position, u_ViewPosition.xyz);
	//float k = floor(log(d / 7.5) / log(2));
	//float texScale = pow(5, k - 2);
	//vec3 uv = texScale * fs_in.Position;
	vec3 uv = fs_in.Position * u_TexScale;

	ivec4 matIndices = u_PerInstanceData[fs_in.MapIndex].mMatIndex;

	vec3 triplanarWeights = GetTriplanarWeights(normal);

	vec2 uvAxisX = uv.zy;
	vec2 uvAxisY = uv.xz;
	vec2 uvAxisZ = uv.xy;

	float height = 0;
	GetMaterials2(matData, height, triplanarWeights, uvAxisX, uvAxisY, uvAxisZ, matIndices, fs_in.Blend);

	matData.mMetallic = clamp(matData.mMetallic, 0, 1);
	matData.mRoughness = clamp(matData.mRoughness, 0, 1);	

	normal = PerturbNormal(fs_in.Position, normal, height);
		
	outNormal = normal;
	EncodeMaterialData(outUI32Buffer1, matData);
}


/*
void GetMaterials(out MaterialData mat, out float height, int matIndex, vec2 texUV, float blend)
{
	mat.mBaseColor = vec3(u_Materials[matIndex].mBaseColorR, u_Materials[matIndex].mBaseColorG, u_Materials[matIndex].mBaseColorB);
	mat.mRoughness = u_Materials[matIndex].mRoughness;
	mat.mMetallic =  u_Materials[matIndex].mMetallic;
	mat.mPorosity =  u_Materials[matIndex].mPorosity;

	int baseColorTextureIndex = u_Materials[matIndex].mBaseColorTextureIndex;
	if (baseColorTextureIndex != -1)
	{
		int baseColorSamplerIndex = u_Materials[matIndex].mBaseColorSamplerIndex;
		mat.mBaseColor = mat.mBaseColor * TexGet(baseColorSamplerIndex, texUV, baseColorTextureIndex).xyz;
	}

	int metallicTextureIndex = u_Materials[matIndex].mMetallicTextureIndex;
	if(metallicTextureIndex != -1)
	{
		int metallicSamplerIndex = u_Materials[matIndex].mMetallicSamplerIndex;
		mat.mMetallic = mat.mMetallic * TexGet(metallicSamplerIndex, texUV, metallicTextureIndex).x;
	}

	int roughnessTextureIndex = u_Materials[matIndex].mRoughnessTextureIndex;
	if (roughnessTextureIndex != -1)
	{
		int roughnessSamplerIndex = u_Materials[matIndex].mRoughnessSamplerIndex;
		mat.mRoughness = mat.mRoughness * TexGet(roughnessSamplerIndex, texUV, roughnessTextureIndex).x;
	}

	int emissiveTextureIndex = u_Materials[matIndex].mEmissiveTextureIndex;
	if(emissiveTextureIndex != -1)
	{
		int emissiveSamplerIndex = u_Materials[matIndex].mEmissiveSamplerIndex;
		mat.mEmissive = u_Materials[matIndex].mEmissive * TexGet(emissiveSamplerIndex, texUV, emissiveTextureIndex).xyz;
	}
	else
	{
		mat.mEmissive =  mat.mBaseColor * u_Materials[matIndex].mEmissive;
	}

	int heightTextureIndex = u_Materials[matIndex].mNormalTextureIndex;
	if ((heightTextureIndex != -1))
	{
		int heightSamplerIndex = u_Materials[matIndex].mNormalSamplerIndex;
		height = TexGet(heightSamplerIndex, texUV, heightTextureIndex).x;
	}
	else
	{
		height = 0;
	}

	mat.mBaseColor *= blend;
	mat.mRoughness *= blend;
	mat.mMetallic *= blend;
	mat.mPorosity *= blend;
	height *= blend;
}
*/