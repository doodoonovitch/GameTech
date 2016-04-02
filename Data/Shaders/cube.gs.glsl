layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int u_MaterialBaseIndex;

uniform samplerBuffer u_perInstanceDataSampler;
uniform isamplerBuffer u_materialIndexSampler;

#define PROPERTY_PER_MATERIAL_COUNT 2


in VS_OUT
{
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	int InstanceId;
} gs_in[3];

out GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ViewModelDQ;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].InstanceId * 2;
	modelDQ.Qr = texelFetch(u_perInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(u_perInstanceDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	int matIndex = texelFetch(u_materialIndexSampler, gs_in[0].InstanceId).r * PROPERTY_PER_MATERIAL_COUNT + u_MaterialBaseIndex;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		gs_out.MaterialIndex = matIndex;
		gs_out.ViewModelDQ = viewModelDQ;
		
		gs_out.Position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		gl_Position = u_ProjMatrix * gs_out.Position;

		gs_out.Normal = gs_in[i].Normal;
		gs_out.Tangent = gs_in[i].Tangent;
		//gs_out.Normal = dqTransformNormal(gs_in[i].Normal.xyz, viewModelDQ);

		gs_out.TexUV = gs_in[i].TexUV;
		EmitVertex();
	}

	EndPrimitive();
}
