#version 420

#define	POSITION_ATTRIBUTE			0
#define	NORMAL_ATTRIBUTE			1
#define	TANGENT_ATTRIBUTE			2
#define	BINORMAL_ATTRIBUTE			3
#define	UV_ATTRIBUTE				4
#define	MATERIALID_ATTRIBUTE		5

#define MAX_LIGHT_COUNT				16

#define POINT_LIGHT_TYPE			0
#define DIRECTIONAL_LIGHT_TYPE		1
#define SPOT_LIGHT_TYPE				2


#define GetLightType(lightDesc)				(lightDesc & 15)
#define GetLightDataIndex(lightDesc)		int((lightDesc >> 16))

layout (std140, shared) uniform FrameData
{
	mat4 u_ViewMatrix;
	mat4 u_ProjMatrix;

	vec4 u_AmbientLight;
		// bits 16 - 31 (16 bits) : index in the light data texture buffer
		// bits 0 - 3 (4 bits) : light type
		// bits 4 - 15 : reserved 

	uint u_LightDesc[MAX_LIGHT_COUNT]; 
	vec4 u_LightData[64];
	uint u_LightCount; 
};

