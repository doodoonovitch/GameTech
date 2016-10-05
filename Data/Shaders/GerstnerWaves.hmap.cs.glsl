#define WAVEPARAM_DIR_X			0
#define WAVEPARAM_DIR_Y			1
#define WAVEPARAM_W				2
#define WAVEPARAM_AMPLITUDE		3
#define WAVEPARAM_PHASE			4
#define WAVEPARAM_STEEPNESS		5

#define WAVEPARAM_ITEMS_COUNT	6


layout (binding = 0, rgba16f) uniform image2D u_ImageOut;
layout (binding = 1, std430) coherent readonly buffer u_WaveParamsBlock
{
	float u_WaveParams[];
};

uniform int u_WaveCount;
uniform ivec2 u_TextureSize;
uniform vec2 u_Scale;
uniform float u_Time;


//layout (local_size_x = 32, local_size_y = 32) in;
layout (local_size_x = 1, local_size_y = 1) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = u_Scale * vec2(TWO_PI) * vec2(p) / vec2(u_TextureSize);

	vec3 P = vec3(0);

	int baseIndex = 0;
	for(int i = 0; i < u_WaveCount; ++i)
	{
		float Q = u_WaveParams[baseIndex + WAVEPARAM_STEEPNESS];
		float W = u_WaveParams[baseIndex + WAVEPARAM_W];
		float A = u_WaveParams[baseIndex + WAVEPARAM_AMPLITUDE];
		float phase = u_WaveParams[baseIndex + WAVEPARAM_PHASE];

		float QA = Q * A;
		float WA = W * A;
		vec2 D = vec2(u_WaveParams[baseIndex + WAVEPARAM_DIR_X], u_WaveParams[baseIndex + WAVEPARAM_DIR_Y]);

		float WDPplusPhase = W * dot(D, tc) + phase * u_Time;
		float S = sin(WDPplusPhase);
		float C = cos(WDPplusPhase);

		float QAC = QA * C;
		vec2 f = vec2(QAC) * D;

		P += vec3(f, A * S);

		baseIndex += WAVEPARAM_ITEMS_COUNT;
	}

	imageStore(u_ImageOut, p, vec4(P, 0));
}