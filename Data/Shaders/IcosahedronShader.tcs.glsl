layout (vertices = 3) out;

uniform float u_InnerTessLevel;
uniform float u_OuterTessLevel;

//out TCS_OUT
//{
//	vec3 Position;
//} tcs_out[];

void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = u_OuterTessLevel;
		gl_TessLevelOuter[1] = u_OuterTessLevel;
		gl_TessLevelOuter[2] = u_OuterTessLevel;
		gl_TessLevelInner[0] = u_InnerTessLevel;
		gl_TessLevelInner[1] = u_InnerTessLevel;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


