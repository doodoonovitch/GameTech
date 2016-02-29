#version 420

#define	POSITION_ATTRIBUTE							0
#define	NORMAL_ATTRIBUTE							1
#define	TANGENT_ATTRIBUTE							2
#define	BINORMAL_ATTRIBUTE							3
#define	UV_ATTRIBUTE								4
#define	MATERIALID_ATTRIBUTE						5

#define MAX_LIGHT_COUNT								16

#define POINT_LIGHT_TYPE							0
#define DIRECTIONAL_LIGHT_TYPE						1
#define SPOT_LIGHT_TYPE								2

#define POINT_LIGHT_COLOR_INDEX						0	
#define POINT_LIGHT_POSITION_INDEX					1	

#define DIRECTIONAL_LIGHT_COLOR_INDEX				0	
#define DIRECTIONAL_LIGHT_DIRECTION_INDEX			1	


#define PI											3.14159265

#define GetLightDesc(index)							u_LightDesc[index >> 2][index & 3]
#define GetLightType(lightDesc)						(lightDesc & 15)
#define GetLightDataIndex(lightDesc)				(lightDesc >> 16)

/*
#define GetAmbientTextureIndex(bitfieldValue)		(bitfieldValue & 0x000000FF)
#define GetDiffuseTextureIndex(bitfieldValue)		((bitfieldValue & 0xFF000000) >> 24)
#define GetSpecularTextureIndex(bitfieldValue)		((bitfieldValue & 0x00FF0000) >> 16)
#define GetNormalTextureIndex(bitfieldValue)		((bitfieldValue & 0x0000FF00) >> 8)
*/

#define GetAmbientTextureIndex(bitfieldValue)		(bitfieldValue & uint(255))
#define GetDiffuseTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 24)) >> 24)
#define GetSpecularTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 16)) >> 16)
#define GetNormalTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 8)) >> 8)

#define when_eq(a, b)								(1 - abs(sign(a - b)))
#define when_ne(a, b)								abs(sign(a - b))
#define when_gt(a, b)								max(sign(a - b), 0)
#define when_lt(a, b)								max(sign(b - a), 0)
#define when_ge(a, b)								(1 - when_lt(a, b))
#define when_le(a, b)								(1 - when_gt(a, b))



layout (std140, shared) uniform FrameData
{
	mat4 u_ViewMatrix;
	mat4 u_ProjMatrix;

	vec4 u_AmbientLight;
		// bits 16 - 31 (16 bits) : index in the light data texture buffer
		// bits 0 - 3 (4 bits) : light type
		// bits 4 - 15 : reserved 

	ivec4 u_LightDesc[MAX_LIGHT_COUNT / 4]; 
	vec4 u_LightData[64];
	int u_LightCount; 
};