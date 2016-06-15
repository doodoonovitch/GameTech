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
	vec4 t = vec4(texelFetch(u_perInstanceDataSampler, index).xyz, 0.f);

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		vec4 viewPos = gl_in[i].gl_Position + t;
		//vec4 viewPos = (gl_in[i].gl_Position * vec4(2.f, 2.f, 2.f, 1.f)) + t;
		//viewPos = vec4(dqTransformPoint(u_ViewDQ, viewPos.xyz), 1.f);
		viewPos = u_ViewMatrix * viewPos;
		gl_Position = u_ProjMatrix * viewPos;

		EmitVertex();
	}

	EndPrimitive();
}
