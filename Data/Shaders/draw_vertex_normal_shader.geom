layout (triangles) in;
layout (line_strip, max_vertices = 104) out;

uniform vec4 u_VertexNormalColor;
uniform vec4 u_PointLightColor;
uniform vec4 u_DirectionalLightColor;
uniform float u_NormalMagnitude;
uniform float u_LightMagnitude;
uniform samplerBuffer perInstanceDataSampler;

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
	int index = gs_in[0].InstanceId * 4;
	vec4 col1 = texelFetch(perInstanceDataSampler, index);
	vec4 col2 = texelFetch(perInstanceDataSampler, index + 1);
	vec4 col3 = texelFetch(perInstanceDataSampler, index + 2);
	vec4 col4 = texelFetch(perInstanceDataSampler, index + 3);
	
	mat4 modelMatrix = mat4(col1, col2, col3, col4);

	mat4 modelViewMatrix = u_ViewMatrix * modelMatrix;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		vec4 position = modelViewMatrix * gl_in[i].gl_Position;
		vec4 projPos = u_ProjMatrix * position;

		gl_Position = projPos;
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		vec4 normal = modelViewMatrix * vec4(gs_in[i].Normal.xyz, 0);
		gl_Position = u_ProjMatrix * (position + (normal * u_NormalMagnitude));
		gs_out.Color = u_VertexNormalColor;
		EmitVertex();

		EndPrimitive();

		for(int lightIndex = 0; lightIndex < u_LightCount; ++lightIndex)
		{
			int lightDesc = GetLightDesc(lightIndex);
			int lightType = GetLightType(lightDesc);
			int dataIndex = GetLightDataIndex(lightDesc);

			if (lightType == POINT_LIGHT_TYPE)
			{
				gl_Position = projPos;
				gs_out.Color = u_PointLightColor;
				EmitVertex();

				vec4 lightPosition = u_LightData[dataIndex + POINT_LIGHT_POSITION_INDEX];
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

				vec4 lightDirection = normalize(u_LightData[dataIndex + DIRECTIONAL_LIGHT_DIRECTION_INDEX]);
				gl_Position = u_ProjMatrix * (position + (lightDirection * u_LightMagnitude));

				gs_out.Color = u_DirectionalLightColor;
				EmitVertex();
				EndPrimitive();
			}
			
		}
	}
}
