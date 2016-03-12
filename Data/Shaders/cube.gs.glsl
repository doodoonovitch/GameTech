layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int u_MaterialBaseIndex;
uniform samplerBuffer perInstanceDataSampler;
uniform isamplerBuffer materialIndexSampler;

#define PROPERTY_PER_MATERIAL_COUNT 3


in VS_OUT
{
	vec3 Normal;
	vec2 TexUV;
	int InstanceId;
} gs_in[3];

out GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].InstanceId * 2;
	modelDQ.Qr = texelFetch(perInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(perInstanceDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	int matIndex = texelFetch(materialIndexSampler, gs_in[0].InstanceId).r * PROPERTY_PER_MATERIAL_COUNT + u_MaterialBaseIndex;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		gs_out.MaterialIndex = matIndex;

		gs_out.Position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		gl_Position = u_ProjMatrix * gs_out.Position;

		gs_out.Normal = dqTransformNormal(gs_in[i].Normal.xyz, viewModelDQ);

		gs_out.TexUV = gs_in[i].TexUV;
		EmitVertex();
	}

	EndPrimitive();
}
