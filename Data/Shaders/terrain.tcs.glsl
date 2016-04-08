layout (vertices = 4) out;

in VS_OUT
{
	vec2 TexUV;
	int Layer;
	int LocalIndex;
} tcs_in[];

out TCS_OUT
{
	vec2 TexUV;
	int Layer;
	int LocalIndex;
} tcs_out[];

void main()
{
	if (gl_InvocationID == 0)
	{
        vec4 p0 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[0].gl_Position.xyz), 1.0);
        vec4 p1 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[1].gl_Position.xyz), 1.0);
        vec4 p2 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[2].gl_Position.xyz), 1.0);
        vec4 p3 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[3].gl_Position.xyz), 1.0);
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;

        if (p0.z <= 0.0 && p1.z <= 0.0 && p2.z <= 0.0 && p3.z <= 0.0)
		{
            gl_TessLevelOuter[0] = 0.0;
            gl_TessLevelOuter[1] = 0.0;
            gl_TessLevelOuter[2] = 0.0;
            gl_TessLevelOuter[3] = 0.0;
        }
		else
		{
            float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;
            float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;
            float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;
            float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;
            gl_TessLevelOuter[0] = l0;
            gl_TessLevelOuter[1] = l1;
            gl_TessLevelOuter[2] = l2;
            gl_TessLevelOuter[3] = l3;
            gl_TessLevelInner[0] = min(l1, l3);
            gl_TessLevelInner[1] = min(l0, l2);
        }
	} 

    tcs_out[gl_InvocationID].TexUV = tcs_in[gl_InvocationID].TexUV;
    tcs_out[gl_InvocationID].Layer = tcs_in[gl_InvocationID].Layer;
	tcs_out[gl_InvocationID].LocalIndex = tcs_in[gl_InvocationID].LocalIndex;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


