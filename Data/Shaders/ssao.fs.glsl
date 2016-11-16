layout(location = 0) out float outFragColor;

uniform sampler2D	u_NBufferSampler;
uniform usampler2D	u_GBuffer1Sampler;
uniform sampler2D	u_DepthSampler;
uniform sampler2D	u_NoiseSampler;
uniform sampler1D	u_KernelSampler;

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
	FragmentInfo fi;
	UnpackFromGBuffer(fi, fs_in.TexUV, fs_in.ViewRay, u_GBuffer1Sampler, u_DepthSampler);

	if (fi.mRendererId == SKYBOX_RENDERER_ID || fi.mRendererId == uint(0))
	{
		outFragColor = 1.0;
		return;
	}

	vec3 normal = normalize(texture(u_NBufferSampler, fs_in.TexUV, 0).xyz);

	vec3 randomVec = texture(u_NoiseSampler, fs_in.TexUV * u_NoiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for(int i = 0; i < u_KernelSize; ++i)
	{
		// get sample position
		vec3 smp = TBN * texelFetch(u_KernelSampler, i, 0).xyz; 
		smp = fi.mPosition + smp * u_Radius; 
        
		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(smp, 1.0);
		offset = u_ProjMatrix * offset; 
		offset.xyz /= offset.w;      
		offset.xyz = offset.xyz * 0.5 + 0.5; 

		float sampleDepth = texture(u_DepthSampler, offset.xy).r; 
		sampleDepth = LinearizeDepth(sampleDepth);
        
		// range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(fi.mPosition.z - sampleDepth));
		occlusion += (sampleDepth >= smp.z ? 1.0 : 0.0) * rangeCheck;

	}
	occlusion = 1.0 - (occlusion / u_KernelSize);

	outFragColor = occlusion;
}