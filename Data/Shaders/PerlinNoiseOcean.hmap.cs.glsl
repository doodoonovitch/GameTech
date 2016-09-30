#define WAVEPARAM_DIR_X			0
#define WAVEPARAM_DIR_Y			1
#define WAVEPARAM_WAVE_LENGTH	2
#define WAVEPARAM_AMPLITUDE		3
#define WAVEPARAM_VELOCITY		4
//struct Param
//{
//	vec2 u_A; // xy : Direction, 
//	vec3 u_B; // x : wave length, y : amplitude, z : velocity
//};

layout (binding = 0, r32f) uniform image2D u_ImageOut;

layout (binding = 1, rgba32f) uniform image2D u_ImageIn;
layout (binding = 2, std430) readonly buffer u_WaveParamsBlock
{
	float u_WaveParams[];
};

uniform int u_WaveCount;
uniform ivec2 u_TextureSize;
uniform float u_Time;

layout (local_size_x = 1, local_size_y = 1) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = vec2(p) / vec2(u_TextureSize);

	float H = 0;

	int baseIndex = 0;
	for(int i = 0; i < u_WaveCount; ++i)
	{
		vec2 uv = 
			tc * u_WaveParams[baseIndex + WAVEPARAM_WAVE_LENGTH] 
			+ u_Time * u_WaveParams[baseIndex + WAVEPARAM_VELOCITY] * vec2(u_WaveParams[baseIndex + WAVEPARAM_DIR_X], u_WaveParams[baseIndex + WAVEPARAM_DIR_Y]);

		H += u_WaveParams[baseIndex + WAVEPARAM_AMPLITUDE] * imageLoad(u_ImageIn, ivec2(round(uv * u_TextureSize))).r;
	}

	imageStore(u_ImageOut, p, vec4(H));
}