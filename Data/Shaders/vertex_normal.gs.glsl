layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform samplerBuffer perInstanceDataSampler;

in VS_OUT
{
	vec3 Normal;
	int InstanceId;
} gs_in[3];

out GS_OUT
{
	vec4 Position;
	vec4 Color;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].InstanceId * 2;
	modelDQ.Qr = texelFetch(perInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(perInstanceDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	for(int i = 0; i < gl_in.length(); ++i )
	{
		vec4 position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		vec4 projPos = u_ProjMatrix * position;

		gl_Position = projPos;
		gs_out.Position = position;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		vec4 normal = vec4(dqTransformNormal(gs_in[i].Normal.xyz, viewModelDQ), 0);
		position = (position + (normal * u_NormalMagnitude));
		gl_Position = u_ProjMatrix * position;
		gs_out.Position = position;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		EndPrimitive();
	}
}
