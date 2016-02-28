layout(location = 0) out vec4 vFragColor;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
} fs_in;

uniform sampler2D texSampler1;


void main(void)
{
	vec4 diffuseMaterial = texture(texSampler1, fs_in.TexUV);
	vec4 diffuseColor = vec4(0, 0, 0, 0);

	vec3 normal = normalize(fs_in.Normal);

	for(int lightIndex = 0; lightIndex < u_LightCount; ++lightIndex)
	{
		int lightDesc = GetLightDesc(lightIndex);
		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		if (lightType == POINT_LIGHT_TYPE)
		{
			vec4 lightColor = u_LightData[dataIndex + POINT_LIGHT_COLOR_INDEX];
			vec4 lightPosition = u_LightData[dataIndex + POINT_LIGHT_POSITION_INDEX];
			vec3 lightDirection = normalize(lightPosition.xyz - fs_in.Position.xyz);
			diffuseColor += max(0, dot(normal, lightDirection)) * lightColor * diffuseMaterial;
		}
		else if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			vec4 lightColor = u_LightData[dataIndex + DIRECTIONAL_LIGHT_COLOR_INDEX];
			vec3 lightDirection = u_LightData[dataIndex + DIRECTIONAL_LIGHT_DIRECTION_INDEX].xyz;
			diffuseColor += max(0, dot(normal, -lightDirection)) * lightColor * diffuseMaterial;
		}
		else
		{
		}
	}
	

	vFragColor = clamp(diffuseMaterial * u_AmbientLight + diffuseColor, 0, 1);
}
