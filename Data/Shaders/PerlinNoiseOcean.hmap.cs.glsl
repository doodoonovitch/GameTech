#version 450 core

struct Param
{
	vec2 u_A; // xy : Direction, 
	vec3 u_B; // x : wave length, y : amplitude, z : velocity
};

layout (binding = 0, r32f) writeonly uniform image2D image_out;

layout (binding = 1) readonly uniform image2D image_in;
layout (binding = 2, std430) uniform Params
{
	Param u_WaveParam[];
};

layout (binding = 3) uniform int u_WaveCount;
layout (binding = 4) uniform ivec2 u_TextureSize;
layout (binding = 5) uniform float u_Time;

layout (local_size_x = 64, local_size_y = 64) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = p / u_TextureSize;

	float H = 0;

	for(int i = 0; i < u_WaveCount; ++i)
	{
		vec2 uv = tc * u_WaveParam[i].u_B.x + u_Time * u_WaveParam[i].u_B.z * u_WaveParam[i].u_A.xy;
		H += u_WaveParam[i].u_B.y * imageLoad(image_in, ivec2(round(uv * u_TextureSize))).r;
	}		 

	imageStore(image_out, p, H);
}