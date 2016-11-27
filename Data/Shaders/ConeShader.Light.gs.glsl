layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform isamplerBuffer u_LightDescSampler;
uniform samplerBuffer u_LightDataSampler;
uniform int u_LightOffset;

in VS_OUT
{
	flat int InstanceId;
} gs_in[3];

out GS_OUT
{
	vec4 Color;
} gs_out;

void main()
{  
	int index = gs_in[0].InstanceId + u_LightOffset;
	int lightDesc = texelFetch(u_LightDescSampler, index).x;

	int dataIndex = GetLightDataIndex(lightDesc);

	vec4 lightColorIntensity = texelFetch(u_LightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
	vec4 lightColor = vec4(lightColorIntensity.xyz * lightColorIntensity.w, 1.f);

	vec4 lightPosition = texelFetch(u_LightDataSampler, dataIndex + SPOT_LIGHT_POSITION_PROPERTY);
	vec3 lightDirection = normalize(texelFetch(u_LightDataSampler, dataIndex + SPOT_LIGHT_DIRECTION_PROPERTY).xyz);
	
	vec4 qRot = qRotationBetweenVectors(vec3(0, -1, 0), lightDirection);
	DualQuat rotDQ = dqFromRotation(qRot);
	DualQuat traDQ = dqFromTranslation(lightPosition.xyz);
	DualQuat modelDQ = dqMul(traDQ, rotDQ);

	for(int i = 0; i < gl_in.length(); ++i )
	{	
		vec4 pos = vec4(dqTransformPoint(modelDQ, gl_in[i].gl_Position.xyz), 1);
		gl_Position = u_ViewProjMatrix * pos;
		gs_out.Color = lightColor;

		EmitVertex();
	}

	EndPrimitive();
}
