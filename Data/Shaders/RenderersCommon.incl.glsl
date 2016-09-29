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
#define FIRST_TEXTURE_SAMPLER_INDEX					10
#define MAX_TEXTURE_SAMPLER							32
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Render ids
//
#define CUBE_RENDERER_ID							1
#define AXIS_RENDERER_ID							2
#define GRID_RENDERER_ID							3
#define VERTEX_NORMAL_RENDERER_ID					4
#define TERRAIN_RENDERER_ID							5
#define DEEPOCEAN_RENDERER_ID						6
#define SKYBOX_RENDERER_ID							7
//
// ---------------------------------------------------------------------------

#define Mask_0x00FFFFFF								uint(16777215)
#define Mask_0xFF000000								uint(uint(255) << 24)
#define Mask_0x00FF0000								uint(uint(255) << 16)
#define Mask_0x0000FF00								uint(uint(255) << 8)
#define Mask_0x000000FF								uint(255)
#define CombineRenderIdAndMaterialIndex(rendererId, materialIndex) uint((rendererId << 24) | (materialIndex & Mask_0x00FFFFFF))



// ===========================================================================
// ===========================================================================
// ===========================================================================



layout (std140, shared) uniform FrameData
{
	mat4 u_ProjMatrix;
	mat4 u_InvProjMatrix;

	mat4 u_OrthoProjMatrix;
		
	mat4 u_ViewMatrix;

	DualQuat u_ViewDQ;

	vec4 u_ViewPosition;

	vec4 u_AmbientLight;

	vec4 u_WireFrameDrawColor;
	vec4 u_VertexNormalColor;

	vec4 u_BufferViewportSize;
	vec4 u_DepthRangeFovYAspect;
	
	vec2 u_TimeDeltaTime;

	float u_NormalMagnitude;
	
	float u_Exposure;

	int u_PointLightCount; 
	int u_SpotLightCount;
	int u_DirectionalLightCount;
};




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


void WriteOutData(out uvec4 outAlbedoAndStatus, out vec4 outSpecularAndRoughness, out vec3 outEmissive,
	int rendererId, vec3 matDiffuseColor, vec3 matSpecularColor, float roughness, vec3 matEmissive)
{
	outAlbedoAndStatus = uvec4(matDiffuseColor * 255, rendererId);
	outSpecularAndRoughness = vec4(matSpecularColor, roughness);
	outEmissive = matEmissive;
}




// ===========================================================================
// ===========================================================================
// ===========================================================================




