layout (vertices = 3) out;

uniform float u_InnerTessLevel;
uniform float u_OuterTessLevel;

in VS_OUT
{
	int InstanceId;
} vs_in[];

out TCS_OUT
{
	int InstanceId;
} tcs_out[];

void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = u_OuterTessLevel;
		gl_TessLevelOuter[1] = u_OuterTessLevel;
		gl_TessLevelOuter[2] = u_OuterTessLevel;
		gl_TessLevelInner[0] = u_InnerTessLevel;
	}

	tcs_out[gl_InvocationID].InstanceId = vs_in[gl_InvocationID].InstanceId;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


