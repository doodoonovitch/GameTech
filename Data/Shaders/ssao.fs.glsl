layout(location = 0) out float outFragColor;

uniform sampler2D	u_gBufferNormal;
uniform sampler2D	u_gDepthMap;
uniform usampler2D	u_gBufferAlbedoAndStatus;
uniform sampler2D	u_gNoiseMap;
uniform sampler1D	u_gKernel;

uniform int			u_KernelSize;
uniform float		u_Radius;
uniform vec2		u_NoiseScale;

in VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} fs_in;


void main(void)
{	
	int rendererId = int(texture(u_gBufferAlbedoAndStatus, fs_in.TexUV, 0).w & 15);
	if (rendererId == SKYBOX_RENDERER_ID || !(rendererId > 0))
	{
		outFragColor = 1.0;
		return;
	}

	vec3 normal = normalize(texture(u_gBufferNormal, fs_in.TexUV, 0).xyz);

	float depth = texture(u_gDepthMap, fs_in.TexUV).r;
	vec3 position = PositionFromDepth(depth, fs_in.ViewRay);

	vec3 randomVec = texture(u_gNoiseMap, fs_in.TexUV * u_NoiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < u_KernelSize; ++i)
	{
		// get sample position
		vec3 sample = TBN * texelFetch(u_gKernel, i).xyz; 
		sample = position + sample * u_Radius; 
        
		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(sample, 1.0);
		offset = u_ProjMatrix * offset; 
		offset.xyz /= offset.w;      
		offset.xyz = offset.xyz * 0.5 + 0.5; 

		float sampleDepth = texture(u_gDepthMap, offset.xy).r; 
		sampleDepth = LinearizeDepth(sampleDepth);
        
		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;           
	}
	occlusion = 1.0 - (occlusion / u_KernelSize);

	outFragColor = occlusion;
}