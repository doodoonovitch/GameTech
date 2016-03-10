layout(location = 0) out vec4 vFragColor;

in vec2 TexUV;

uniform usampler2D gBuffer0;
uniform sampler2D gBuffer1;

uniform samplerBuffer materialDataSampler;
uniform isamplerBuffer lightDescSampler;
uniform samplerBuffer lightDataSampler;


struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec2 TexUV;
    uint MaterialIndex;
};

void UnpackGBuffer(ivec2 coord, out FragmentInfo fragment)
{
    uvec4 data0 = texelFetch(gBuffer0, ivec2(coord), 0);
    vec4 data1 = texelFetch(gBuffer0, ivec2(coord), 0);
    vec2 temp;

    temp = unpackHalf2x16(data0.y);
    fragment.Normal = vec3(temp.y, data1.w);
    fragment.MaterialIndex = data0.w;

    fragment.Position = data1.xyz;
}


void main(void)
{
	FragmentInfo fs_in;
	UnpackGBuffer(TexUV, fs_in);

	vec4 v = texelFetch(materialDataSampler, fs_in.MaterialIndex);
	vec4 materialAmbient = vec4(v.xyz, 1);

	uint bitfieldValue = floatBitsToUint(v.w);
	int ambientTextureIndex = int(GetAmbientTextureIndex(bitfieldValue));
	int diffuseTextureIndex = int(GetDiffuseTextureIndex(bitfieldValue));
	int specularTextureIndex = int(GetSpecularTextureIndex(bitfieldValue));

	v = texelFetch(materialDataSampler, fs_in.MaterialIndex + 1);
	vec4 materialDiffuse = vec4(v.xyz, 1);

	v = texelFetch(materialDataSampler, fragInfo.MaterialIndex + 2);
	vec4 materialSpecular = vec4(v.xyz, 1);

	float materialShininess = v.w;

	if (ambientTextureIndex != 0x000000FF)
	{
		materialAmbient = materialAmbient * texture(textureSampler, fs_in.TexUV);
	}
	
	if (diffuseTextureIndex != 0x000000FF)
	{
		materialDiffuse = materialDiffuse * texture(textureSampler, fs_in.TexUV);
	}

	if (specularTextureIndex != 0x000000FF)
	{
		materialSpecular = materialSpecular * texture(textureSampler, fs_in.TexUV);
	}
	
	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 specularColor = vec3(0, 0, 0);
	vec3 diffuseColor = vec3(0, 0, 0);

	vec3 normal = normalize(fs_in.Normal);
	vec3 viewDirection = normalize(-fs_in.Position.xyz);

	int lightCount = texelFetch(lightDescSampler, 0).x;
	for(int lightIndex = 0; lightIndex < lightCount; ++lightIndex)
	{
		int lightDesc = texelFetch(lightDescSampler, lightIndex + 1).x;
		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec3 lightAmbient = texelFetch(lightDataSampler, dataIndex + LIGHT_AMBIENT_PROPERTY).xyz;
		vec3 lightDiffuse = texelFetch(lightDataSampler, dataIndex + LIGHT_DIFFUSE_PROPERTY).xyz;
		vec3 lightSpecular = texelFetch(lightDataSampler, dataIndex + LIGHT_SPECULAR_PROPERTY).xyz;

		float attenuation = 1.0;
		vec3 lightDirection;
		vec3 reflectionDirection;

		if (lightType == POINT_LIGHT_TYPE)
		{
			vec4 lightPosition = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);
			vec4 attenuationCoef = texelFetch(lightDataSampler, dataIndex + POINT_LIGHT_ATTENUATION_PROPERTY);

			lightDirection = lightPosition.xyz - fs_in.Position.xyz;
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

			reflectionDirection = reflect(-lightDirection, normal);
		}
		else if (lightType == DIRECTIONAL_LIGHT_TYPE)
		{
			lightDirection = -texelFetch(lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;

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
