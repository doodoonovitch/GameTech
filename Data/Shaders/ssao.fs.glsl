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
	//FragmentInfo fi;
	//UnpackFromGBuffer(fi, fs_in.TexUV, fs_in.ViewRay, u_GBuffer1Sampler, u_DepthSampler);

	uvec4 inUI32Buffer1;
	inUI32Buffer1 = texture(u_GBuffer1Sampler, fs_in.TexUV, 0);
	uint rendererId = inUI32Buffer1.x & Mask_0x000000FF;
	
	if (rendererId == SKYBOX_RENDERER_ID || rendererId == uint(0))
	{
		outFragColor = 1.0;
		return;
	}

	float depthNDC = texture(u_DepthSampler, fs_in.TexUV, 0).r * 2.0 - 1.0;
	vec4 posV = vec4(PositionFromDepth(depthNDC, fs_in.ViewRay), 1);
	vec3 position = (u_InvViewMatrix * posV).xyz;

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
		smp = position + smp * u_Radius; 
        
		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(smp, 1.0);
		offset = u_ViewProjMatrix * offset; 
		offset.xyz /= offset.w;      
		offset.xy = offset.xy * 0.5 + 0.5; 

		float smpZ = LinearizeDepth(offset.z);

		float sampleDepthN = texture(u_DepthSampler, offset.xy).r * 2.0 - 1.0; 
		float sampleDepth = LinearizeDepth(sampleDepthN);

        
		// range check & accumulate
		//float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(posV.z - sampleDepth));
		//occlusion += (sampleDepth >= smp.z ? 1.0 : 0.0) * rangeCheck;
		float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(posV.z - sampleDepth));
		occlusion += (sampleDepth >= smpZ ? 1.0 : 0.0) * rangeCheck;

	}
	occlusion = 1.0 - (occlusion / u_KernelSize);

	outFragColor = occlusion;
}