layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int u_MaterialBaseIndex;

uniform samplerBuffer u_PerInstanceDataSampler;
uniform isamplerBuffer u_PerInstanceDataIndexSampler;

#define PROPERTY_PER_MATERIAL_COUNT 3


in VS_OUT
{
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	int InstanceId;
	int MeshId;
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
	int offset = texelFetch(u_PerInstanceDataIndexSampler, gs_in[0].MeshId).r;
	int index = (offset + gs_in[0].InstanceId) * 3;
	
	DualQuat modelDQ;
	modelDQ.Qr = texelFetch(u_PerInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(u_PerInstanceDataSampler, index + 1);
	vec3 scale = texelFetch(u_PerInstanceDataSampler, index + 2).xyz;
	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	int matIndex = gs_in[0].MeshId * PROPERTY_PER_MATERIAL_COUNT + u_MaterialBaseIndex;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		gs_out.MaterialIndex = matIndex;
		gs_out.ViewModelDQ = viewModelDQ;
		
		vec3 position = scale * gl_in[i].gl_Position.xyz;
		gs_out.Position = vec4(dqTransformPoint(viewModelDQ, position), 1);
		gl_Position = u_ProjMatrix * gs_out.Position;

		gs_out.Normal = scale * gs_in[i].Normal; 
		gs_out.Tangent = scale * gs_in[i].Tangent; 

		gs_out.TexUV = gs_in[i].TexUV;
		EmitVertex();
	}

	EndPrimitive();
}
