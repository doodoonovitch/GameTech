layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} fs_in;


uniform sampler2D u_NBufferSampler;
uniform usampler2D u_GBuffer1Sampler;
uniform sampler2D u_DepthSampler;
uniform sampler2D u_SSAOSampler;

uniform int u_BufferToShow;




//
// ---------------------------------------------------------------------------

#define NORMAL_BUFFER		1
#define ALBEDO_BUFFER		2
#define SPECULAR_BUFFER		3
#define ROUGHNESS_BUFFER	4
#define POSITION_BUFFER		5
#define SSAO_BUFFER			6
#define EMISSIVE_BUFFER		7
#define DEPTH_BUFFER		8


void DrawEmissiveBuffer(vec3 emissive)
{
	vec3 mapped = vec3(1.0) - exp(-emissive * u_Exposure);
	mapped = pow(mapped, vec3(u_InvGamma));
	vFragColor =  vec4(mapped, 1.0);

	//vFragColor = vec4(fi.mEmissive, 1.f);
}

void main(void)
{
	FragmentInfo fi;

	UnpackFromGBuffer(fi, fs_in.TexUV, fs_in.ViewRay, u_GBuffer1Sampler, u_DepthSampler);

	fi.mNormal = normalize(texture(u_NBufferSampler, fs_in.TexUV, 0).xyz);

	//if (fi.mRendererId == SKYBOX_RENDERER_ID)
	//{
	//	DrawEmissiveBuffer(fi.mEmissive);
	//}
	if (fi.mRendererId > 0)
	{
		if(u_BufferToShow == NORMAL_BUFFER)
		{
			vFragColor = vec4(fi.mNormal, 1);
		}
		else if(u_BufferToShow == ALBEDO_BUFFER)
		{
			vFragColor = vec4(fi.mBaseColor, 1);
		}
		else if(u_BufferToShow == SPECULAR_BUFFER)
		{
			vFragColor = vec4(fi.mMetallic, 0, 0, 1);
		}
		else if(u_BufferToShow == ROUGHNESS_BUFFER)
		{
			vFragColor = vec4(0, fi.mRoughness, 0, 1);
		}
		else if(u_BufferToShow == POSITION_BUFFER)
		{
			vFragColor = vec4(fi.mPosition, 1);
		}
		else if(u_BufferToShow == SSAO_BUFFER)
		{
			float occlusion = texture(u_SSAOSampler, fs_in.TexUV, 0).r;
			vFragColor = vec4(occlusion, occlusion, occlusion, 1);
		}
		else if(u_BufferToShow == EMISSIVE_BUFFER)
		{
			vFragColor = vec4(fi.mEmissive, 1);
		}
		else if(u_BufferToShow == DEPTH_BUFFER)
		{
			vFragColor = vec4(vec3(1 - fi.mDepthNDC), 1);
		}
		
	}
	else
	{
		vFragColor = vec4(0.f, 0.f, 0.f, 1.f);
	}
}

