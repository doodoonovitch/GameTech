// ---------------------------------------------------------------------------
// Vertex attribute locations
//
#define	POSITION_ATTRIBUTE							0
#define	UV_ATTRIBUTE								1
#define	NORMAL_ATTRIBUTE							2
#define	TANGENT_ATTRIBUTE							3
#define	MESHID_ATTRIBUTE							4
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Light property indices
//
#define POINT_LIGHT_TYPE							0
#define DIRECTIONAL_LIGHT_TYPE						1
#define SPOT_LIGHT_TYPE								2

#define LIGHT_COLOR_PROPERTY						0	

#define POINT_LIGHT_POSITION_PROPERTY				1
#define POINT_LIGHT_WORLD_POSITION_PROPERTY			2

#define SPOT_LIGHT_POSITION_PROPERTY				1
#define SPOT_LIGHT_DIRECTION_PROPERTY				2
#define SPOT_LIGHT_WORLD_POSITION_PROPERTY			3
#define SPOT_LIGHT_WORLD_DIRECTION_PROPERTY			4

#define DIRECTIONAL_LIGHT_DIRECTION_PROPERTY		1
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Light helper macros
//
#define GetLightType(lightDesc)						(lightDesc & 15)
#define GetLightDataIndex(lightDesc)				(lightDesc >> 16)
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Material sampler first index and max sampler count
//
#define MAX_TEXTURE_SAMPLER							32
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Render ids
//
#define MODEL_RENDERER_ID							1
#define AXIS_RENDERER_ID							2
#define GRID_RENDERER_ID							3
#define VERTEX_NORMAL_RENDERER_ID					4
#define TERRAIN_RENDERER_ID							5
#define DEEPOCEAN_RENDERER_ID						6
#define SKYBOX_RENDERER_ID							7
#define DEBUG_RENDERER_ID							8

//
// ---------------------------------------------------------------------------

#define Mask_0x00FFFFFF								uint(16777215)
#define Mask_0xFFFFFF00								uint(4294967040)
#define Mask_0xFFFF0000								uint(4294901760)
#define Mask_0xFF000000								uint(uint(255) << 24)
#define Mask_0x00FF0000								uint(uint(255) << 16)
#define Mask_0x0000FF00								uint(uint(255) << 8)
#define Mask_0x000000FF								uint(255)
#define Mask_0x0000FFFF								uint(65535)



// ===========================================================================
// ===========================================================================
// ===========================================================================


struct ModelMaterial 
{
	float mBaseColorR;
	float mBaseColorG;
	float mBaseColorB;

	float mMetallic;
	float mRoughness;
	float mPorosity;
	float mEmissive;
	float mHeightOffset;
	float mHeightScale;

	int mBaseColorSamplerIndex, mBaseColorTextureIndex;
	int mMetallicSamplerIndex, mMetallicTextureIndex;
	int mRoughnessSamplerIndex, mRoughnessTextureIndex;
	int mNormalSamplerIndex, mNormalTextureIndex;
	int mEmissiveSamplerIndex, mEmissiveTextureIndex;
	int mHeightSamplerIndex, mHeightTextureIndex;
};





// ===========================================================================
// ===========================================================================
// ===========================================================================


// ---------------------------------------------------------------------------
// Linearize depth
//
// ---------------------------------------------------------------------------
float LinearizeDepth(float depthNDC)
{
	float n = u_NearFarFovYAspect.x;
	float f = u_NearFarFovYAspect.y;
	//depthNDC = depthNDC * 2.0 - 1.0;
	float zView = 2 * f * n / (depthNDC * (f - n) - (f + n));

	return zView;
}

// ---------------------------------------------------------------------------
// Position reconstruction from depth
//
// ---------------------------------------------------------------------------
vec3 PositionFromDepth(float depthNDC, vec2 viewRay)
{

	float zView = LinearizeDepth(depthNDC);
	vec2 xyView = viewRay * -zView;

	return vec3(xyView, zView);
}




// ===========================================================================
// ===========================================================================
// ===========================================================================




vec3 GetBumpedNormal(sampler2DArray gNormalMap, vec3 texCoord)
{
    vec3 bumpMapNormal = texture(gNormalMap, texCoord).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
	return bumpMapNormal;
}

vec3 ComputeBumpedNormal(vec3 normal, vec3 tangent, vec3 bumpMapNormal)
{
	normal = normalize(normal);
	tangent = normalize(tangent);
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 newNormal;
	newNormal = TBN * normalize(bumpMapNormal);
    newNormal = normalize(newNormal);
    return newNormal;
}

vec3 ComputeBumpedNormal(vec3 normal, vec3 tangent, sampler2DArray gNormalMap, vec3 texCoord)
{
	vec3 bumpMapNormal = GetBumpedNormal(gNormalMap, texCoord);
	return ComputeBumpedNormal(normal, tangent, bumpMapNormal);
}







// ===========================================================================
// ===========================================================================
// ===========================================================================




float AngleAttenuation(in vec3 lightDirection, in vec3 spotDirection, in float innerConeCos, in float outerConeCos)
{
	float theta = dot(lightDirection, spotDirection); 
	float epsilon = max(innerConeCos - outerConeCos, 0.001);
	float spotIntensity = clamp((theta - outerConeCos) / epsilon, 0.0, 1.0);
	return spotIntensity;
}

float DistAttenuation(in vec3 unnormalizedLightVector, in float lightRadius)
{
	//float lightDistance = length(unnormalizedLightVector);
	//float fact1 = saturate(1 - pow(lightDistance / lightRadius, 4));
	//float fact2 = 1 / (lightDistance * lightDistance + 1);
	//float attenuation = fact1 * fact1 * fact2;

	float dist2 = dot(unnormalizedLightVector, unnormalizedLightVector);
	float invDist2 = 1 / max(dist2, 0.01 * 0.01);
	int N = 4;
	float lightRadiusN = pow(lightRadius, N);
	float distN = pow(dist2, N /2);
	float fact2 = saturate(1 - (distN / lightRadiusN));
	float attenuation = invDist2 * fact2 * fact2;
	return attenuation;
}




// ===========================================================================
// ===========================================================================
// ===========================================================================



// ===============================================================================
// Calculates the Fresnel factor using Schlick's approximation
//
//		float LoH = saturate(dot(l, h));
// ===============================================================================
float BRDF_SchlickFresnel(in float LoH)
{
    float m = clamp(1 - LoH, 0, 1);
	return Pow5(m);
}

vec3 BRDF_SchlickFresnel(in vec3 specularColor, in float LoH)
{
    float m = clamp(1 - LoH, 0, 1);
	float fh = Pow5(m);

	return specularColor + (vec3(1.0f) - specularColor) * fh;
}

// ---------------------------------------------------------------------------
// BRDF_LambertDiffuse(Kd)
//
// ---------------------------------------------------------------------------
vec3 BRDF_LambertDiffuse(in vec3 Kd)
{
	return Kd / PI;
}

// ---------------------------------------------------------------------------
// BRDF_BlinnPhongDiffuse(Kd, LdotH)
//
// ---------------------------------------------------------------------------
vec3 BRDF_BlinnPhongDiffuse(in vec3 Kd, in vec3 fresnel)
{
	return (vec3(1.0f) -  fresnel) * Kd / PI;
}

vec3 BRDF_Specular_F_Roughness(vec3 specularColor, float a, float NoV)
{
    float m = 1 - clamp(NoV, 0, 1);
	float fh = Pow5(m);

    return (specularColor + (max(vec3(1.0f - a), specularColor) - specularColor) * fh);
}