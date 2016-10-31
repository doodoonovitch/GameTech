
layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;
layout (location = NORMAL_ATTRIBUTE) in vec3 in_Normal;
layout (location = TANGENT_ATTRIBUTE) in vec3 in_Tangent;
layout (location = UV_ATTRIBUTE) in vec2 in_TexUV;
#ifdef ARB_SHADER_DRAW_PARAMETERS
#else
layout (location = MESHID_ATTRIBUTE) in int in_MeshId;
#endif

out VS_OUT
{
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	int InstanceId;
	int MeshId;
} vs_out;

void main()
{  
	vs_out.Normal = in_Normal;
	vs_out.Tangent = in_Tangent;
	vs_out.TexUV = in_TexUV;
	vs_out.InstanceId = gl_InstanceID;

#ifdef ARB_SHADER_DRAW_PARAMETERS
	vs_out.MeshId = gl_BaseInstanceARB;
#else
	vs_out.MeshId = in_MeshId;
#endif

	gl_Position = vec4(in_Position.xyz, 1);
}

