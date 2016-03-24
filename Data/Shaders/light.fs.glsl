layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


uniform sampler2D u_gBufferPosition;
uniform usampler2D u_gBufferRGBA32UI;

uniform samplerBuffer u_materialDataSampler;
uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;

uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec3 Albedo;
    int MaterialIndex;
};

void UnpackGBuffer(vec2 coord, out FragmentInfo fragment)
{
    fragment.Position = texture(u_gBufferPosition, coord, 0).xyz;
    uvec4 data = texture(u_gBufferRGBA32UI, coord, 0);

    vec2 temp = unpackHalf2x16(data.y);
    fragment.Albedo.xy = unpackHalf2x16(data.x);
    fragment.Albedo.z = temp.x;

    fragment.Normal.xy = unpackHalf2x16(data.z);
    fragment.Normal.z = temp.y;

    fragment.MaterialIndex = int(data.w);
}

void main(void)
{
	FragmentInfo gData;
	UnpackGBuffer(fs_in.TexUV, gData);
	
	vec4 matData = texelFetch(u_materialDataSampler, gData.MaterialIndex);
	vec4 materialAmbient = vec4(matData.xyz, 1);
	uint bitfieldValue = floatBitsToUint(matData.w);
	int ambientTextureIndex = int(GetAmbientTextureIndex(bitfieldValue));
	int diffuseTextureIndex = int(GetDiffuseTextureIndex(bitfieldValue));
	int specularTextureIndex = int(GetSpecularTextureIndex(bitfieldValue));

	matData = texelFetch(u_materialDataSampler, gData.MaterialIndex + 1);
	vec4 materialDiffuse = vec4(matData.xyz, 1);
	bitfieldValue = floatBitsToUint(matData.w);
	int ambientSamplerIndex = int(GetAmbientSamplerIndex(bitfieldValue));
	int diffuseSamplerIndex = int(GetDiffuseSamplerIndex(bitfieldValue));
	int specularSamplerIndex = int(GetSpecularSamplerIndex(bitfieldValue));

	matData = texelFetch(u_materialDataSampler, gData.MaterialIndex + 2);
	vec4 materialSpecular = vec4(matData.xyz, 1);
	float materialShininess = matData.w;

	if (ambientTextureIndex != 0x000000FF)
	{
		materialAmbient = materialAmbient * texture(u_textureSampler[/*ambientSamplerIndex*/0], vec3(fs_in.TexUV, ambientTextureIndex));
	}
	
	if (diffuseTextureIndex != 0x000000FF)
	{
		materialDiffuse = materialDiffuse * texture(u_textureSampler[/*diffuseSamplerIndex*/0], vec3(fs_in.TexUV, diffuseTextureIndex));
	}

	if (specularTextureIndex != 0x000000FF)
	{
		materialSpecular = materialSpecular * texture(u_textureSampler[/*specularTextureIndex*/0], vec3(fs_in.TexUV, specularTextureIndex));
	}

	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 specularColor = vec3(0, 0, 0);
	vec3 diffuseColor = vec3(0, 0, 0);

	vec3 normal = normalize(gData.Normal);
	vec3 viewDirection = normalize(-gData.Position.xyz);

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

			lightDirection = lightPosition.xyz - gData.Position.xyz;
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
	//vFragColor = vec4(gData.Position, 1);
}
