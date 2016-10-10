layout (binding = 0, rgba8_snorm) uniform image2D u_ImageOut;

uniform ivec2 u_TextureSize;
uniform sampler2D u_HeightMapSampler;


layout (local_size_x = 1, local_size_y = 1) in;
//layout (local_size_x = 32, local_size_y = 32) in;
//layout (local_size_x = MAX_COMPUTE_WORKGROUP_SIZE_X, local_size_y = MAX_COMPUTE_WORKGROUP_SIZE_Y) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = p / vec2(u_TextureSize);

	float nX = textureOffset(u_HeightMapSampler, tc, ivec2(-1, 0)).r - textureOffset(u_HeightMapSampler, tc, ivec2(1, 0)).r;
	float nZ = textureOffset(u_HeightMapSampler, tc, ivec2(0, -1)).r - textureOffset(u_HeightMapSampler, tc, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX, 2, nZ));

	imageStore(u_ImageOut, p, vec4(normal, 0));
}