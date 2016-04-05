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


// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------

void UnpackPositionAndMaterialIdFromGBuffer(out vec3 Position, out int MaterialIndex, out int RendererId)
{
    vec4 pos = texture(u_gBufferPosition, fs_in.TexUV, 0);
	Position = pos.xyz;
	uint bitfieldValue = floatBitsToUint(pos.w);
	MaterialIndex = int(bitfieldValue & uint(0x00FFFFFF));
	RendererId = int((bitfieldValue & uint(0xFF000000)) >> 24);
}

void UnpackNormalTexUVFromGBuffer(out vec3 Normal, out vec2 TexUV)
{
    uvec2 data = texture(u_gBufferData, fs_in.TexUV, 0).xy;
    TexUV.xy = unpackHalf2x16(data.x);
    Normal.xy = unpackHalf2x16(data.y);
	Normal.z = sqrt(dot(Normal.xy, Normal.xy));
	Normal = normalize(Normal);
}

//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Renderers
//
// ---------------------------------------------------------------------------

vec4 CubeRenderer(vec3 Position, int MaterialIndex)
{
    vec3 normal;
	vec2 texUV;

	UnpackNormalTexUVFromGBuffer(normal, texUV);

	vec4 matData = texelFetch(u_materialDataSampler, MaterialIndex);
	vec4 materialDiffuse = vec4(matData.xyz, 1);
	uint bitfieldValue = floatBitsToUint(matData.w);
	int diffuseTextureIndex = int((bitfieldValue >> 16) & uint(255));
	int diffuseSamplerIndex = int((bitfieldValue >> 24) & uint(255));
	int specularTextureIndex = int(bitfieldValue & uint(255));
	int specularSamplerIndex = int((bitfieldValue >> 8) & uint(255));

	matData = texelFetch(u_materialDataSampler, MaterialIndex + 1);
	vec4 materialSpecular = vec4(matData.xyz, 1);
	bitfieldValue = floatBitsToUint(matData.w);
	float materialShininess = float((bitfieldValue >> 16) & uint(65535));

	if (diffuseTextureIndex != -1)
	{
		materialDiffuse = materialDiffuse * TexGet(diffuseSamplerIndex, vec3(texUV, diffuseTextureIndex));
	}

	if (specularTextureIndex != -1)
	{
		materialSpecular = materialSpecular * TexGet(specularSamplerIndex, vec3(texUV, specularTextureIndex));
	}

	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 diffuseColor = vec3(0, 0, 0);
	vec3 specularColor = vec3(0, 0, 0);

	vec3 viewDirection = normalize(-Position.xyz);

	int lightIndex = 1;

	// point light evaluation
	for(int pointLight = 0; pointLight < u_PointLightCount; ++pointLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec4 lightColorIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
		vec3 lightColor = lightColorIntensity.xyz;
		float lightIntensity = lightColorIntensity.w;

		vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);
		vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_ATTENUATION_PROPERTY);

		vec3 lightDirection = lightPosition.xyz - Position.xyz;
		float lightDistance = length(lightDirection);
		lightDirection = lightDirection / lightDistance;

		float attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float ambientFactor = attenuation * lightIntensity;
		ambientColor += lightColor.xyz * ambientFactor;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * attenuation * lightIntensity;
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * attenuation * lightIntensity;	
		specularColor += lightColor * specularFactor;
	}
	
	// spot light evaluation
	for(int spotLight = 0; spotLight < u_SpotLightCount; ++spotLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec4 lightColorIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
		vec3 lightColor = lightColorIntensity.xyz;
		float lightIntensity = lightColorIntensity.w;


		vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_POSITION_PROPERTY);
		vec4 spotDirection = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_DIRECTION_PROPERTY);
		vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_ATTENUATION_PROPERTY);
		float innerConeCos = spotDirection.w;
		float outerConeCos = attenuationCoef.w;

		vec3 lightDirection = lightPosition.xyz - Position.xyz;
		float lightDistance = length(lightDirection);
		lightDirection = lightDirection / lightDistance;

		float attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

		vec3 reflectionDirection = reflect(-lightDirection, normal);

		float theta = dot(lightDirection, normalize(-spotDirection.xyz)); 
		float epsilon = (innerConeCos - outerConeCos);
		float spotIntensity = clamp((theta - outerConeCos) / epsilon, 0.0, 1.0);

		attenuation = attenuation * spotIntensity;

		float ambientFactor = attenuation * lightIntensity;
		ambientColor += lightColor * ambientFactor;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * attenuation * lightIntensity;
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * attenuation * lightIntensity;
		specularColor += lightColor * specularFactor;
	}
	
	// Directional light evaluation
	for(int dirLight = 0; dirLight < u_DirectionalLightCount; ++dirLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec4 lightColorIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
		vec3 lightColor = lightColorIntensity.xyz;
		float lightIntensity = lightColorIntensity.w;

		vec3 lightDirection;
		vec3 reflectionDirection;

		lightDirection = -texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;
		reflectionDirection = reflect(lightDirection, normal);

		float ambientFactor = lightIntensity;
		ambientColor += lightColor * ambientFactor;

		float diffuseFactor = max(0, dot(normal, lightDirection)) * lightIntensity;
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), materialShininess) * lightIntensity;	
		specularColor += lightColor * specularFactor;
	}
	
	return  materialDiffuse * vec4(ambientColor *.5 + diffuseColor, 1) +  materialSpecular * vec4(specularColor, 1);
}

//
// ---------------------------------------------------------------------------


void main(void)
{
	vec3 position;
	int materialIndex;
	int rendererId;

	UnpackPositionAndMaterialIdFromGBuffer(position, materialIndex, rendererId);
	
	if(rendererId == CUBE_RENDERER_ID)
	{
		vFragColor = CubeRenderer(position, materialIndex);
	}
}

