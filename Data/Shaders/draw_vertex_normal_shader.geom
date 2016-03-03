layout (triangles) in;
layout (line_strip, max_vertices = 114) out;

uniform vec4 u_VertexNormalColor;
uniform vec4 u_PointLightColor;
uniform vec4 u_DirectionalLightColor;
uniform float u_NormalMagnitude;
uniform float u_LightMagnitude;
uniform int u_FirstLightIndex;
uniform int u_DrawLightCount;

uniform samplerBuffer perInstanceDataSampler;
uniform isamplerBuffer lightDescSampler;
uniform samplerBuffer lightDataSampler;

#define MAX_LIGHT_COUNT 16

in VS_OUT
{
	vec3 Normal;
	int InstanceId;
} gs_in[3];

out GS_OUT
{
	vec4 Color;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].InstanceId * 2;
	modelDQ.Qr = texelFetch(perInstanceDataSampler, index);
	modelDQ.Qd = texelFetch(perInstanceDataSampler, index + 1);

	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

	int lightCount = texelFetch(lightDescSampler, 0).r;
	int lightToDraw = min(lightCount, min(u_DrawLightCount, MAX_LIGHT_COUNT));

	for(int i = 0; i < gl_in.length(); ++i )
	{
		vec4 position = vec4(dqTransformPoint(viewModelDQ, gl_in[i].gl_Position.xyz), 1);
		vec4 projPos = u_ProjMatrix * position;

		gl_Position = projPos;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		vec4 normal = vec4(dqTransformNormal(gs_in[i].Normal.xyz, viewModelDQ), 0);
		gl_Position = u_ProjMatrix * (position + (normal * u_NormalMagnitude));
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		EndPrimitive();

		for(int lightIndex = 0; lightIndex < lightToDraw; ++lightIndex)
		{
			int lightDesc = texelFetch(lightDescSampler, ((lightIndex + u_FirstLightIndex) % lightCount) + 1).r;
			int lightType = GetLightType(lightDesc);
			int dataIndex = GetLightDataIndex(lightDesc);

			if (lightType == POINT_LIGHT_TYPE)
			{
				gl_Position = projPos;
				gs_out.Color = u_PointLightColor;
				EmitVertex();

				vec4 lightPosition = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_POSITION_INDEX);
				vec4 lightDirection = normalize(lightPosition - position);
				gl_Position = u_ProjMatrix * (position + (lightDirection * u_LightMagnitude));

				gs_out.Color = u_PointLightColor;
				EmitVertex();
				EndPrimitive();
			}
			
			else if (lightType == DIRECTIONAL_LIGHT_TYPE)
			{
				gl_Position = projPos;
				gs_out.Color = u_DirectionalLightColor;
				EmitVertex();

				vec4 lightDirection = -texelFetch(lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_INDEX);
				gl_Position = u_ProjMatrix * (position + (lightDirection * u_LightMagnitude));

				gs_out.Color = u_DirectionalLightColor;
				EmitVertex();
				EndPrimitive();
			}
			
		}
	}
}
