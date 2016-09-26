#version 450 core

layout (local_size_x = 64, local_size_y = 64) in;

layout (binding = 0) readonly uniform image2D image_in;
layout (binding = 1, r32f) writeonly uniform image2D image_out;

void main(void)
{
	vec4 texel;
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	texel = imageLoad(image_in, p);
	texel = vec4(1.0) - texel;
	imageStore(image_out, p, texel);
}