layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//uniform int u_MaterialBaseIndex;

uniform samplerBuffer u_PerMapDataSampler;
//uniform isamplerBuffer u_materialIndexSampler;


in TES_OUT
{
	vec2 TexUV;
	vec3 Normal;
	flat int MapIndex;
} gs_in[3];

out GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	//vec3 Normal;
	vec3 WorldNormal;
	vec3 ViewNormal;
	//flat int MapIndex;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].MapIndex * 2;
	modelDQ.Qr = texelFetch(u_PerMapDataSampler, index);
	modelDQ.Qd = texelFetch(u_PerMapDataSampler, index + 1);

	//gs_out.ViewModelDQ = dqMul(u_ViewDQ, modelDQ);
	
	for(int i = 0; i < gl_in.length(); ++i )
	{	
		//gs_out.TexUV = gs_in[i].TexUV;
		gs_out.WorldPosition = dqTransformPoint(modelDQ, gl_in[i].gl_Position.xyz);

		vec4 viewPos = vec4(dqTransformPoint(u_ViewDQ, gs_out.WorldPosition), 1.0);
		gs_out.ViewPosition = viewPos.xyz;
		gl_Position = u_ProjMatrix * viewPos;

		//gs_out.Normal = gs_in[i].Normal;
		gs_out.WorldNormal = dqTransformNormal(gs_in[i].Normal, modelDQ);
		gs_out.ViewNormal = dqTransformNormal(gs_out.WorldNormal, u_ViewDQ);

		EmitVertex();
	}

	EndPrimitive();
}
