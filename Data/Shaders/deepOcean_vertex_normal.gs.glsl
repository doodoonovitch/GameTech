layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform samplerBuffer u_PerMapDataSampler;

in TES_OUT
{
	//vec2 TexUV;
	vec3 Normal;
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
		vec4 position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		vec4 projPos = u_ProjMatrix * position;

		gl_Position = projPos;
		gs_out.Position = position;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		position = gl_in[i].gl_Position + vec4(gs_in[i].Normal * u_NormalMagnitude, 0);
		position = vec4(dqTransformPoint(viewModelDQ, position.xyz), 1);
		gl_Position = u_ProjMatrix * position;
		gs_out.Position = position;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		EndPrimitive();
	}
}
