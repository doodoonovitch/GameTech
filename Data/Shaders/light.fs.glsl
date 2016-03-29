layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


uniform sampler2D u_gBufferPosition;
uniform usampler2D u_gBufferData;

uniform samplerBuffer u_materialDataSampler;
uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;

uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(int samplerIndex, vec3 p);

struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec2 TexUV;
    int MaterialIndex;
};

void UnpackGBuffer(vec2 coord, out FragmentInfo fragment)
{
    vec4 pos = texture(u_gBufferPosition, coord, 0);
	fragment.Position = pos.xyz;
	fragment.MaterialIndex = floatBitsToInt(pos.w);

    uvec2 data = texture(u_gBufferData, coord, 0).xy;
    fragment.TexUV.xy = unpackHalf2x16(data.x);
    fragment.Normal.xy = unpackHalf2x16(data.y);
	fragment.Normal.z = sqrt(dot(fragment.Normal.xy, fragment.Normal.xy));

}

void main(void)
{
	FragmentInfo gData;
	UnpackGBuffer(fs_in.TexUV, gData);
	
	vec4 matData = texelFetch(u_materialDataSampler, gData.MaterialIndex);
	vec4 materialAmbient = vec4(matData.xyz, 1);
	uint bitfieldValue = floatBitsToUint(matData.w);
	int ambientTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int ambientSamplerIndex = int((bitfieldValue >> 24) & uint(255));

	matData = texelFetch(u_materialDataSampler, gData.MaterialIndex + 1);
	vec4 materialDiffuse = vec4(matData.xyz, 1);
	bitfieldValue = floatBitsToUint(matData.w);
	int diffuseTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int diffuseSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	int specularTextureIndex = int(bitfieldValue & uint(255));
	int specularSamplerIndex = int((bitfieldValue >> 8) & uint(255));

	matData = texelFetch(u_materialDataSampler, gData.MaterialIndex + 2);
	vec4 materialSpecular = vec4(matData.xyz, 1);
	float materialShininess = matData.w;

	if ((ambientTextureIndex & 0x000000FF)!= 0x000000FF)
	{
		materialAmbient = materialAmbient * TexGet(ambientSamplerIndex, vec3(gData.TexUV, ambientTextureIndex));
	}
	
	if ((diffuseTextureIndex & 0x000000FF)!= 0x000000FF)
	{
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec3(gData.TexUV, diffuseTextureIndex));
	}

	if ((specularTextureIndex & 0x000000FF)!= 0x000000FF)
	{
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec3(gData.TexUV, specularTextureIndex));
	}

	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 diffuseColor = vec3(0, 0, 0);
	vec3 specularColor = vec3(0, 0, 0);

	vec3 normal = normalize(gData.Normal);
	vec3 viewDirection = normalize(-gData.Position.xyz);

	int lightIndex = 1;

	// point light evaluation
	for(int pointLight = 0; pointLight < u_PointLightCount; ++pointLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec3 lightColor = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY).xyz;
		vec3 lightIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_INTENSITY_PROPERTY).xyz;

		float attenuation = 1.0;
		vec3 lightDirection;
		vec3 reflectionDirection;

			vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);
			vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_ATTENUATION_PROPERTY);

			lightDirection = lightPosition.xyz - gData.Position.xyz;
			float lightDistance = length(lightDirection);
			lightDirection = lightDirection / lightDistance;

			attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

			reflectionDirection = reflect(-lightDirection, normal);

		float ambientFactor = lightIntensity.x * attenuation * lightIntensity.x;
		ambientColor += lightColor * ambientFactor;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * attenuation * lightIntensity.y;		
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * attenuation * lightIntensity.z;	
		specularColor += lightColor * specularFactor;
	}

	// Directional light evaluation
	for(int dirLight = 0; dirLight < u_DirectionalLightCount; ++dirLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec3 lightColor = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY).xyz;
		vec3 lightIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_INTENSITY_PROPERTY).xyz;

		float attenuation = 1.0;
		vec3 lightDirection;
		vec3 reflectionDirection;

		lightDirection = -texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;
		reflectionDirection = reflect(lightDirection, normal);

		float ambientFactor = lightIntensity.x * attenuation * lightIntensity.x;
		ambientColor += lightColor * ambientFactor;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * attenuation * lightIntensity.y;		
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * attenuation * lightIntensity.z;	
		specularColor += lightColor * specularFactor;
	}
	
	vFragColor =  clamp(materialAmbient * vec4(ambientColor, 1) + materialDiffuse * vec4(diffuseColor,1) + materialSpecular * vec4(specularColor, 1), 0, 1);
}
