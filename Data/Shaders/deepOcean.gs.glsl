layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform samplerBuffer u_PerMapDataSampler;


in TES_OUT
{
	vec3 Normal;
	//vec3 Tangent;
	flat int MapIndex;
} gs_in[3];

out GS_OUT
{
	vec3 Position;
	vec3 ViewPosition;
	vec3 Normal;
	//vec3 Tangent;
	//flat DualQuat ViewModelDQ;
	//flat int MapIndex;
} gs_out;

void main()
{  
	//DualQuat modelDQ;
	//int index = gs_in[0].MapIndex * 2;
	//modelDQ.Qr = texelFetch(u_PerMapDataSampler, index);
	//modelDQ.Qd = texelFetch(u_PerMapDataSampler, index + 1);

	//gs_out.ViewModelDQ = dqMul(u_ViewDQ, modelDQ);

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		//gs_out.TexUV = gs_in[i].TexUV;
		gs_out.Position = gl_in[i].gl_Position.xyz;
		gs_out.Normal = gs_in[i].Normal;
		//gs_out.Tangent = gs_in[i].Tangent;
		//gs_out.WorldPosition = dqTransformPoint(modelDQ, gl_in[i].gl_Position.xyz);

		//vec4 viewPos = vec4(dqTransformPoint(u_ViewDQ, gs_out.WorldPosition), 1.0);
		//vec4 viewPos = vec4(dqTransformPoint(gs_out.ViewModelDQ, gl_in[i].gl_Position.xyz), 1);
		vec4 viewPos = u_ViewMatrix * gl_in[i].gl_Position;
		gs_out.ViewPosition = viewPos.xyz;
		gl_Position = u_ProjMatrix * viewPos;

		//gs_out.WorldNormal = dqTransformNormal(gs_in[i].Normal, modelDQ);
		//gs_out.ViewNormal = dqTransformNormal(gs_out.WorldNormal, u_ViewDQ);

		EmitVertex();
	}

	EndPrimitive();
}
