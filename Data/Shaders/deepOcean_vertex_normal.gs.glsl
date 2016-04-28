layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform samplerBuffer u_PerMapDataSampler;

in TES_OUT
{
	vec3 Normal;
	vec3 Tangent;
	flat int MapIndex;
} gs_in[3];

out GS_OUT
{
	vec4 Position;
	vec4 Color;
} gs_out;

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
		vec4 projPos = u_ProjMatrix * viewPos;

		gl_Position = projPos;
		gs_out.Position = viewPos;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		//viewPos = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz + gs_in[i].Normal * u_NormalMagnitude), 1);
		viewPos += vec4(dqTransformNormal(gs_in[i].Normal * u_NormalMagnitude, viewModelDQ), 0);
		projPos = u_ProjMatrix * viewPos;

		gl_Position = projPos;
		gs_out.Position = viewPos;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		EndPrimitive();
	}
}
