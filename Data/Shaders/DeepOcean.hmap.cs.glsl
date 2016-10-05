#define WAVEPARAM_DIR_X			0
#define WAVEPARAM_DIR_Y			1
#define WAVEPARAM_W				2
#define WAVEPARAM_AMPLITUDE		3
#define WAVEPARAM_PHASE			4
#define WAVEPARAM_STEEPNESS		5

#define WAVEPARAM_ITEMS_COUNT	6


#ifdef PRECOMPUTE_NORMAL
layout (binding = 0, rgba16f) uniform image2D u_ImageOut;
#else
layout (binding = 0, r16f) uniform image2D u_ImageOut;
#endif
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
	vec2 tc = 0.5 * vec2(TWO_PI) * vec2(p) / vec2(u_TextureSize);

	float H = 0;
	vec2 dH = vec2(0);

	int baseIndex = 0;
	for(int i = 0; i < u_WaveCount; ++i)
	{
		float steepness = u_WaveParams[baseIndex + WAVEPARAM_STEEPNESS];
		float frequency = u_WaveParams[baseIndex + WAVEPARAM_W];
		float amplitude = u_WaveParams[baseIndex + WAVEPARAM_AMPLITUDE];

		vec2 dir = vec2(u_WaveParams[baseIndex + WAVEPARAM_DIR_X], u_WaveParams[baseIndex + WAVEPARAM_DIR_Y]);

		float dirPos = dot(dir, tc);
		float S = dirPos * frequency + u_Time * u_WaveParams[baseIndex + WAVEPARAM_PHASE];

		float halfOfSinSplusOne = 0.5f * (1.0f + sin(S));
		H += (amplitude * pow(halfOfSinSplusOne, steepness));
		
#ifdef PRECOMPUTE_NORMAL
		float cosS = cos(S);
		float dhCommon = steepness * frequency * amplitude * cosS;

		if (steepness != 1)
		{
			float halfOfSinSplusOnePowSteepnessMinusOne = pow(halfOfSinSplusOne, steepness - 1);
			dhCommon *= halfOfSinSplusOnePowSteepnessMinusOne;
		}

		dH += vec2(dir.x * dhCommon, dir.y * dhCommon);
#endif

		baseIndex += WAVEPARAM_ITEMS_COUNT;
	}

#ifdef PRECOMPUTE_NORMAL
	vec3 n = normalize(vec3(dH.x, dH.y, 1));
	imageStore(u_ImageOut, p, vec4(H, n.x, n.y, 0));
#else
	imageStore(u_ImageOut, p, vec4(H));
#endif
}