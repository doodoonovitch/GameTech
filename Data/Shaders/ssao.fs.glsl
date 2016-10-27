layout(location = 0) out float outFragColor;

uniform sampler2D u_gBufferNormal;
uniform sampler2D u_gDepthMap;
uniform usampler2D u_gBufferAlbedoAndStatus;
uniform sampler2D u_gNoiseMap;
uniform sampler1D u_gKernel;

uniform int u_KernelSize;
uniform float u_Radius;
uniform vec2 u_NoiseScale;
uniform vec2 u_TextureSize;



void main(void)
{	
	vec3 normal = normalize(texture(u_gBufferNormal, fs_in.TexUV, 0).xyz);
	float depth = texture(u_gDepthMap, fs_in.TexUV).r;
	vec3 position = PositionFromDepth(depth, viewRay);
}