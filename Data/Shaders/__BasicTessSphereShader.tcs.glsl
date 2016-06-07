layout (vertices = 4) out;

uniform float u_TessFactor;

in VS_OUT
{
	vec3 Center;
	float Radius;
} tcs_in[];

out TCS_OUT
{
	vec3 Center;
	float Radius;
} tcs_out[];


void main()
{
	if (gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 2;
		gl_TessLevelOuter[1] = u_TessFactor;
		gl_TessLevelOuter[2] = 2;
		gl_TessLevelOuter[3] = u_TessFactor;
		gl_TessLevelInner[0] = u_TessFactor;
		gl_TessLevelInner[1] = u_TessFactor;
	}

    tcs_out[gl_InvocationID].Center = tcs_in[gl_InvocationID].Center
    tcs_out[gl_InvocationID].Radius = tcs_in[gl_InvocationID].Radius;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


