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
	vec3 Normal;
	//vec3 Tangent;
	//flat int MapIndex;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].MapIndex * 2;
	modelDQ.Qr = texelFetch(u_PerMapDataSampler, index);
	modelDQ.Qd = texelFetch(u_PerMapDataSampler, index + 1);

	//DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);
	//DualQuat viewModelDQ = u_ViewDQ;

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		//gs_out.TexUV = gs_in[i].TexUV;
		gs_out.Position = gl_in[i].gl_Position.xyz;
		gs_out.Normal = gs_in[i].Normal;
		//gs_out.ViewModelDQ = viewModelDQ;

		gl_Position = u_ViewProjMatrix * vec4(gs_out.Position, 1);

		EmitVertex();
	}

	EndPrimitive();
}
