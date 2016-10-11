layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


uniform sampler2D u_gBufferPosition;
uniform sampler2D u_gBufferNormal;
uniform usampler2D u_gBufferAlbedoAndStatus;
uniform sampler2D u_gBufferSpecularRoughness;
uniform sampler2D u_gBufferEmissive;

uniform int u_BufferToShow;

struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec3 DiffuseMaterial;
	vec3 SpecularMaterial;
	vec3 EmissiveMaterial;
	float Roughness;
	//float GlossPower;
	int RendererId;
};


// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi)
{
	fi.Position = texture(u_gBufferPosition, fs_in.TexUV, 0).xyz;
	fi.Normal = texture(u_gBufferNormal, fs_in.TexUV, 0).xyz;

	uvec4 data = texture(u_gBufferAlbedoAndStatus, fs_in.TexUV, 0);
	fi.RendererId = int(data.w & 15);

	uvec3 matD = uvec3(data.x & Mask_0x000000FF, (data.x >> 8) & Mask_0x000000FF, (data.x >> 16) & Mask_0x000000FF);
	fi.DiffuseMaterial = data.xyz / 255.f;

	vec4 temp;
	
	temp = texture(u_gBufferSpecularRoughness, fs_in.TexUV, 0);
	fi.Roughness = temp.w;
	fi.SpecularMaterial = temp.xyz;

	fi.EmissiveMaterial = texture(u_gBufferEmissive, fs_in.TexUV, 0).xyz;
}



//
// ---------------------------------------------------------------------------

#define NORMAL_BUFFER		1
#define ALBEDO_BUFFER		2
#define SPECULAR_BUFFER		3
#define ROUGHNESS_BUFFER	4


void main(void)
{
	FragmentInfo fi;

	UnpackFromGBuffer(fi);

	if (fi.RendererId == SKYBOX_RENDERER_ID)
	{
		vec3 mapped = vec3(1.0) - exp(-fi.SpecularMaterial * u_Exposure);
		mapped = pow(mapped, vec3(u_InvGamma));
		vFragColor =  vec4(mapped, 1.0);
	}
	else
	{
		if(u_BufferToShow == NORMAL_BUFFER)
		{
			vFragColor = vec4(fi.Normal, 1);
		}
		else if(u_BufferToShow == ALBEDO_BUFFER)
		{
			vFragColor = vec4(fi.DiffuseMaterial, 1);
		}
		else if(u_BufferToShow == SPECULAR_BUFFER)
		{
			vFragColor = vec4(fi.SpecularMaterial, 1);
		}
		else if(u_BufferToShow == ROUGHNESS_BUFFER)
		{
			vFragColor = vec4(fi.Roughness, fi.Roughness, fi.Roughness, 1);
		}
	}
}

