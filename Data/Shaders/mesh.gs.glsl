layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std430, binding = 0) buffer PerInstanceDataBuffer
{
	vec4 data[];
} u_PerInstanceDataBuffer;

layout (std430, binding = 1) buffer PerInstanceDataIndexBuffer
{
	int data[];
} u_PerInstanceDataIndexBuffer;

uniform int u_MaterialBaseIndex;

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
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ViewModelDQ;
} gs_out;

void main()
{  
	int offset = u_PerInstanceDataIndexBuffer.data[gs_in[0].MeshId];
	int index = (offset + gs_in[0].InstanceId) * 3;
	
	DualQuat viewModelDQ;
	viewModelDQ.Qr = u_PerInstanceDataBuffer.data[index];
	viewModelDQ.Qd = u_PerInstanceDataBuffer.data[index + 1];
	vec3 scale = u_PerInstanceDataBuffer.data[index + 2].xyz;

	int matIndex = gs_in[0].MeshId * PROPERTY_PER_MATERIAL_COUNT + u_MaterialBaseIndex;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		gs_out.MaterialIndex = matIndex;
		gs_out.ViewModelDQ = viewModelDQ;
		
		vec3 position = scale * gl_in[i].gl_Position.xyz;
		vec4 pos = vec4(dqTransformPoint(viewModelDQ, position), 1);
		gl_Position = u_ProjMatrix * pos;

		gs_out.Normal = scale * gs_in[i].Normal; 
		gs_out.Tangent = scale * gs_in[i].Tangent; 

		gs_out.TexUV = gs_in[i].TexUV;
		EmitVertex();
	}

	EndPrimitive();
}
