layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//uniform int u_MaterialBaseIndex;

//uniform samplerBuffer u_perInstanceDataSampler;
//uniform isamplerBuffer u_materialIndexSampler;


in TES_OUT
{
	vec2 TexUV;
	vec3 Normal;
	flat int Layer;
} gs_in[3];

out GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	//vec2 TexUV;
	vec3 Normal;
	vec3 ViewNormal;
	flat int Layer;
} gs_out;

void main()
{  
	//DualQuat modelDQ;
	//int index = gs_in[0].InstanceId * 2;
	//modelDQ.Qr = texelFetch(u_perInstanceDataSampler, index);
	//modelDQ.Qd = texelFetch(u_perInstanceDataSampler, index + 1);

	//DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);
	DualQuat viewModelDQ = u_ViewDQ;

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		//gs_out.TexUV = gs_in[i].TexUV;
		gs_out.WorldPosition = gl_in[i].gl_Position.xyz;

		vec4 viewPos = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1.0);
		gs_out.ViewPosition = viewPos.xyz;
		gl_Position = u_ProjMatrix * viewPos;

		gs_out.Normal = gs_in[i].Normal;
		gs_out.ViewNormal = dqTransformNormal(gs_in[i].Normal, viewModelDQ);

		EmitVertex();
	}

	EndPrimitive();
}
