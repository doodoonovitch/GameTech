layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform samplerBuffer perInstanceDataSampler;
uniform isamplerBuffer materialIndexSampler;


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
	//int index = gs_in[0].InstanceId * 4;
	//vec4 col1 = texelFetch(perInstanceDataSampler, index);
	//vec4 col2 = texelFetch(perInstanceDataSampler, index + 1);
	//vec4 col3 = texelFetch(perInstanceDataSampler, index + 2);
	//vec4 col4 = texelFetch(perInstanceDataSampler, index + 3);
	//mat4 modelMatrix = mat4(col1, col2, col3, col4);
	//mat4 modelViewMatrix = u_ViewMatrix * modelMatrix;

	DualQuat modelDQ;
	int index = gs_in[0].InstanceId * 2;
	modelDQ.Qr = texelFetch(perInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(perInstanceDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	int matIndex = texelFetch(materialIndexSampler, gs_in[0].InstanceId).x * 3;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		gs_out.MaterialIndex = matIndex;

		//gs_out.Position = modelViewMatrix * gl_in[i].gl_Position;

		gs_out.Position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		gl_Position = u_ProjMatrix * gs_out.Position;

		//vec4 normal = modelViewMatrix * vec4(gs_in[i].Normal.xyz, 0);
		gs_out.Normal = dqTransformNormal(gs_in[i].Normal.xyz, viewModelDQ);

		gs_out.TexUV = gs_in[i].TexUV;
		EmitVertex();
	}

	EndPrimitive();
}
