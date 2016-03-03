layout(location = 0) out vec4 vFragColor;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;

uniform sampler2D texSampler1;
uniform samplerBuffer materialDataSampler;
uniform isamplerBuffer lightDescSampler;
uniform samplerBuffer lightDataSampler;

void main(void)
{
	int matIndex = fs_in.MaterialIndex;

	vec4 v = texelFetch(materialDataSampler, matIndex);
	vec4 materialAmbient = vec4(v.xyz, 1);

	uint bitfieldValue = floatBitsToUint(v.w);
	int ambientTextureIndex = int(GetAmbientTextureIndex(bitfieldValue));
	int diffuseTextureIndex = int(GetDiffuseTextureIndex(bitfieldValue));
	int specularTextureIndex = int(GetSpecularTextureIndex(bitfieldValue));

	v = texelFetch(materialDataSampler, matIndex + 1);
	vec4 materialDiffuse = vec4(v.xyz, 1);

	v = texelFetch(materialDataSampler, matIndex + 2);
	vec4 materialSpecular = vec4(v.xyz, 1);

	float materialShininess = v.w;
	//float energyConservation = ( 2.0 + materialShininess ) / ( 2.0 * PI ); 

	if (ambientTextureIndex != 0x000000FF)
	{
		materialAmbient = materialAmbient * texture(texSampler1, fs_in.TexUV);
	}
	
	if (diffuseTextureIndex != 0x000000FF)
	{
		materialDiffuse = materialDiffuse * texture(texSampler1, fs_in.TexUV);
	}

	if (specularTextureIndex != 0x000000FF)
	{
		materialSpecular = materialSpecular * texture(texSampler1, fs_in.TexUV);
	}
	
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec4 specularColor = vec4(0, 0, 0, 0);

	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDirection = normalize(-fs_in.Position.xyz);

	int lightCount = texelFetch(lightDescSampler, 0).r;
	for(int lightIndex = 0; lightIndex < lightCount; ++lightIndex)
	{
		int lightDesc = texelFetch(lightDescSampler, lightIndex + 1).r;
		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);


		if (lightType == POINT_LIGHT_TYPE)
		{
			vec4 lightColor = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_COLOR_INDEX);
			vec4 lightPosition = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_POSITION_INDEX);
			vec4 lightAttenuation = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_ATTENUATION_INDEX);

			vec3 lightDirection = lightPosition.xyz - fs_in.Position.xyz;
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			lightAttenuation.w = 1.0 / (lightAttenuation.x + lightAttenuation.y * lightDistance + lightAttenuation.z * lightDistance * lightDistance);

			float diffuseFactor = max(0, dot(normal, lightDirection)) * lightAttenuation.w;
			diffuseColor += diffuseFactor * lightColor;

			vec3 reflectionDirection = reflect(-lightDirection, normal);
			specularColor += (pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * when_gt(diffuseFactor, 0)) * lightColor  * lightAttenuation.w;
		}
		else if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			vec4 lightColor = texelFetch(lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_COLOR_INDEX);
			vec3 lightDirection = texelFetch(lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_INDEX).xyz;

			float diffuseFactor = max(0, dot(normal, -lightDirection));
			diffuseColor += diffuseFactor * lightColor;

			vec3 reflectionDirection = reflect(lightDirection, normal);
			specularColor += (pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * when_gt(diffuseFactor, 0)) * lightColor;
		}
		else
		{
		}
	}
	

	vFragColor = clamp(materialAmbient * u_AmbientLight + materialDiffuse * diffuseColor + specularColor * materialSpecular, 0, 1);
}
