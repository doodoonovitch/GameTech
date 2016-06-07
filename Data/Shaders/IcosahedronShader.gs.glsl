layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform samplerBuffer u_PerMapDataSampler;

//in TES_OUT
//{
//	vec3 PatchDistance;
//} gs_in[3];

//out GS_OUT
//{
//	vec3 Position;
//} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].MapIndex * 2;
	modelDQ.Qr = texelFetch(u_PerMapDataSampler, index);
	modelDQ.Qd = texelFetch(u_PerMapDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		vec4 viewPos = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		//gs_out.ViewPosition = viewPos.xyz;
		gl_Position = u_ProjMatrix * viewPos;

		EmitVertex();
	}

	EndPrimitive();
}
