layout(location = 0) out vec4 vFragColor;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;

uniform sampler2D u_textureSampler;
uniform samplerBuffer u_materialDataSampler;
uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;

void main(void)
{
	int matIndex = fs_in.MaterialIndex;

	vec4 v = texelFetch(u_materialDataSampler, matIndex);
	vec4 materialAmbient = vec4(v.xyz, 1);

	uint bitfieldValue = floatBitsToUint(v.w);
	int ambientTextureIndex = int(GetAmbientTextureIndex(bitfieldValue));
	int diffuseTextureIndex = int(GetDiffuseTextureIndex(bitfieldValue));
	int specularTextureIndex = int(GetSpecularTextureIndex(bitfieldValue));

	v = texelFetch(u_materialDataSampler, matIndex + 1);
	vec4 materialDiffuse = vec4(v.xyz, 1);

	v = texelFetch(u_materialDataSampler, matIndex + 2);
	vec4 materialSpecular = vec4(v.xyz, 1);

	float materialShininess = v.w;

	if (ambientTextureIndex != 0x000000FF)
	{
		materialAmbient = materialAmbient * texture(u_textureSampler, fs_in.TexUV);
	}
	
	if (diffuseTextureIndex != 0x000000FF)
	{
		materialDiffuse = materialDiffuse * texture(u_textureSampler, fs_in.TexUV);
	}

	if (specularTextureIndex != 0x000000FF)
	{
		materialSpecular = materialSpecular * texture(u_textureSampler, fs_in.TexUV);
	}
	
	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 specularColor = vec3(0, 0, 0);
	vec3 diffuseColor = vec3(0, 0, 0);

	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDirection = normalize(-fs_in.Position.xyz);

	int lightCount = texelFetch(u_lightDescSampler, 0).x;
	for(int lightIndex = 0; lightIndex < lightCount; ++lightIndex)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex + 1).x;
		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec3 lightAmbient = texelFetch(u_lightDataSampler, dataIndex + LIGHT_AMBIENT_PROPERTY).xyz;
		vec3 lightDiffuse = texelFetch(u_lightDataSampler, dataIndex + LIGHT_DIFFUSE_PROPERTY).xyz;
		vec3 lightSpecular = texelFetch(u_lightDataSampler, dataIndex + LIGHT_SPECULAR_PROPERTY).xyz;

		float attenuation = 1.0;
		vec3 lightDirection;
		vec3 reflectionDirection;

		if (lightType == POINT_LIGHT_TYPE)
		{
			vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);
			vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_ATTENUATION_PROPERTY);

			lightDirection = lightPosition.xyz - fs_in.Position.xyz;
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

			reflectionDirection = reflect(-lightDirection, normal);
		}
		else if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			lightDirection = -texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;

			reflectionDirection = reflect(lightDirection, normal);
		}
		else
		{
			continue;
		}

		ambientColor += lightAmbient * attenuation;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * attenuation;
		diffuseColor += diffuseFactor * lightDiffuse;

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * attenuation;
		specularColor += specularFactor * lightSpecular;

	}
	
	vFragColor =  clamp(materialAmbient * vec4(ambientColor, 1) + materialDiffuse * vec4(diffuseColor,1) + materialSpecular * vec4(specularColor, 1), 0, 1);
}
