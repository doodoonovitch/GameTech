layout (binding = 0, r16f) uniform image2D u_ImageOut;

uniform sampler2D u_ImageIn;

uniform ivec2 u_TexOffset;
uniform float u_Weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

//layout (local_size_x = 32, local_size_y = 32) in;
layout (local_size_x = 1, local_size_y = 1) in;

void main(void)
{
	ivec2 p = ivec2(gl_GlobalInvocationID.xy);
	vec2 tc = vec2(p) / vec2(textureSize(u_ImageIn, 0));

    float result = texture(u_ImageIn, tc).r * u_Weight[0]; // current fragment's contribution

	vec2 texOffset = u_TexOffset / vec2(textureSize(u_ImageIn, 0));

    for(int i = 1; i < 5; ++i)
    {
        result += texture(u_ImageIn, tc + i * texOffset).r * u_Weight[i];
        result += texture(u_ImageIn, tc - i * texOffset).r * u_Weight[i];
    }

	imageStore(u_ImageOut, p, vec4(result));
}