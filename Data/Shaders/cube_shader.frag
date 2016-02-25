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
	vec4 diffuse = vec4(0, 0, 0, 0);

	vec3 normal = normalize(fs_in.Normal);

	for(int i = 0; i < u_LightCount; ++i)
	{
		uint lightDesc = u_LightDesc[i];
		uint lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		if (lightType == POINT_LIGHT_TYPE)
		{
			vec4 lightColor = u_LightData[dataIndex];
			vec4 lightPosition = u_LightData[dataIndex + 1];
			vec3 lightDirection = normalize(lightPosition.xyz - fs_in.Position.xyz);
			diffuse += max(0, dot(normal, lightDirection)) * lightColor * diffuseMaterial;
		}
		else if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			vec4 lightColor = u_LightData[dataIndex];
			vec3 lightDirection = u_LightData[dataIndex + 1].xyz;
			diffuse += max(0, dot(normal, lightDirection)) * lightColor * diffuseMaterial;
		}
		else
		{
		}
	}
	

	vFragColor = clamp(diffuseMaterial * u_AmbientLight + diffuse, 0, 1);
}
