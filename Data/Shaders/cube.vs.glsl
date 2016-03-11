
layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;
layout (location = NORMAL_ATTRIBUTE) in vec3 in_Normal;
layout (location = UV_ATTRIBUTE) in vec2 in_TexUV;

out VS_OUT
{
	vec3 Normal;
	vec2 TexUV;
	int InstanceId;
} vs_out;

void main()
{  
	vs_out.Normal = in_Normal;
	vs_out.TexUV = in_TexUV;
	vs_out.InstanceId = gl_InstanceID;
	gl_Position = vec4(in_Position.xyz, 1);
}

