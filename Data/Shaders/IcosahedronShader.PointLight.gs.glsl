layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform samplerBuffer u_perInstanceDataSampler;

in TES_OUT
{
	//vec3 PatchDistance;
	flat int InstanceId;
} gs_in[3];

//out GS_OUT
//{
//	vec3 Position;
//} gs_out;

void main()
{  
	int index = gs_in[0].InstanceId;
	vec4 t = vec4(dqTransformNormal(texelFetch(u_perInstanceDataSampler, index).xyz, u_ViewDQ), 0);

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		//vec4 viewPos = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		//gs_out.ViewPosition = viewPos.xyz;
		//vec4 viewPos = dqTransformPoint(u_viewDQ, (gl_in[i].gl_Position + t).xyz);
		//vec4 viewPos = gl_in[i].gl_Position + t;
		vec4 viewPos = gl_in[i].gl_Position;
		gl_Position = u_ProjMatrix * viewPos;

		EmitVertex();
	}

	EndPrimitive();
}
