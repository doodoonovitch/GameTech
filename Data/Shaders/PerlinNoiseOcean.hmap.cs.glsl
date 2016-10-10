#define WAVEPARAM_DIR_X			0
#define WAVEPARAM_DIR_Y			1
#define WAVEPARAM_WAVE_LENGTH	2
#define WAVEPARAM_AMPLITUDE		3
#define WAVEPARAM_VELOCITY		4

#define WAVEPARAM_ITEMS_COUNT	5

layout (binding = 0, r16f) uniform image2D u_ImageOut;
layout (binding = 1, std430) coherent readonly buffer u_WaveParamsBlock
{
	float u_WaveParams[];
};

uniform int u_WaveCount;
uniform ivec2 u_TextureSize;
uniform vec2 u_Scale;
uniform float u_Time;
uniform sampler2D u_NoiseSampler;


layout (local_size_x = 1, local_size_y = 1) in;
//layout (local_size_x = 32, local_size_y = 32) in;
//layout (local_size_x = MAX_COMPUTE_WORKGROUP_SIZE_X, local_size_y = MAX_COMPUTE_WORKGROUP_SIZE_Y) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = u_Scale * p / vec2(u_TextureSize);

	float H = 0;

	int baseIndex = 0;
	for(int i = 0; i < u_WaveCount; ++i)
	{
		vec2 uv = 
			tc * u_WaveParams[baseIndex + WAVEPARAM_WAVE_LENGTH]
			+ u_Time * u_WaveParams[baseIndex + WAVEPARAM_VELOCITY] * vec2(u_WaveParams[baseIndex + WAVEPARAM_DIR_X], u_WaveParams[baseIndex + WAVEPARAM_DIR_Y]);

		H += u_WaveParams[baseIndex + WAVEPARAM_AMPLITUDE] * texture(u_NoiseSampler, uv).r;
		baseIndex += WAVEPARAM_ITEMS_COUNT;
	}

	imageStore(u_ImageOut, p, vec4(H));
}