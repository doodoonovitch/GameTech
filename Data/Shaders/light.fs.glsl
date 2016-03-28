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
