layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

out VS_OUT
{
	int InstanceId;
} vs_out;

void main()
{  
	vs_out.InstanceId = gl_InstanceID;
	gl_Position = vec4(in_Position, 1.f);
}

