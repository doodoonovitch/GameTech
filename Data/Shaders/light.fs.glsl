layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} fs_in;

uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;

uniform sampler2D u_DepthSampler;
uniform sampler2D u_NBufferSampler;
uniform usampler2D u_GBuffer1Sampler;
uniform sampler2D u_SSAOSampler;

uniform samplerCube u_EnvMap1Sampler;
uniform samplerCube u_EnvMap2Sampler;

uniform bool u_HasEnvMap;
uniform bool u_IsEnvMapHDR;
uniform bool u_IsSSAOEnabled;



// ---------------------------------------------------------------------------
// BRDF
//
// ---------------------------------------------------------------------------
vec3 BRDF(in vec3 diffuseColor, in vec3 specularColor, in float roughness, in vec3 l, in vec3 v, in vec3 n, in float NoV, in vec3 lightColorIntensity, in float attenuation);

//
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// Renderers
//
// ---------------------------------------------------------------------------

void GetCommonLightProperties(in int lightIndex, out int dataIndex, out vec3 lightColorIntensity, out float lightRadius)
{
	int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
	dataIndex = GetLightDataIndex(lightDesc);

	vec4 data = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
	lightColorIntensity = data.xyz;
	lightRadius = data.w;
}

vec3 BRDF_PointLight(in vec3 v, in float NoV, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);

	vec3 unnormalizedLightVector = lightPosition.xyz - fi.mPosition.xyz;
	vec3 l = normalize(unnormalizedLightVector);
	
	float attenuation = DistAttenuation(unnormalizedLightVector, lightRadius);

	return BRDF(fi.mDiffuseColor, fi.mSpecularColor, fi.mRoughness, l, v, fi.mNormal, NoV, lightColorIntensity, attenuation);
}

vec3 BRDF_SpotLight(in vec3 v, in float NoV, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_POSITION_PROPERTY);
	float outerConeCos = lightPosition.w;
	lightPosition.w = 1.0f;

	vec3 unnormalizedLightVector = lightPosition.xyz - fi.mPosition.xyz;
	vec3 l = normalize(unnormalizedLightVector);

	vec4 spotData = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_DIRECTION_PROPERTY);
	vec3 spotDirection = normalize(-spotData.xyz);
	float innerConeCos = spotData.w;
	
	float attenuation = DistAttenuation(unnormalizedLightVector, lightRadius);
	attenuation *= AngleAttenuation(l, spotDirection, innerConeCos, outerConeCos);

	return BRDF(fi.mDiffuseColor, fi.mSpecularColor, fi.mRoughness, l, v, fi.mNormal, NoV, lightColorIntensity, attenuation);
}

vec3 BRDF_DirectionalLight(in vec3 v, in float NoV, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec3 l = texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;

	return BRDF(fi.mDiffuseColor, fi.mSpecularColor, fi.mRoughness, l, v, fi.mNormal, NoV, lightColorIntensity, 1);
}


vec4 BRDFLight(FragmentInfo fi)
{
	vec3 v = normalize(u_ViewPosition.xyz - fi.mPosition);
	float NoV = max(dot(fi.mNormal, v), 0.0001f);

    fi.mDiffuseColor = fi.mBaseColor - fi.mBaseColor * fi.mMetallic;
    // 0.03 default specular value for dielectric.
    fi.mSpecularColor = mix(vec3(0.03f), fi.mBaseColor, fi.mMetallic);


	int lightIndex = 1;

	vec3 color = vec3(0.f);

	// point light evaluation
	for(int pointLight = 0; pointLight < u_PointLightCount; ++pointLight)
	{
		color += BRDF_PointLight(v, NoV, fi, lightIndex);
		++lightIndex;
	}

	// spot light evaluation
	for(int spotLight = 0; spotLight < u_SpotLightCount; ++spotLight)
	{
		color += BRDF_SpotLight(v, NoV, fi, lightIndex);
		++lightIndex;
	}

	// directional light evaluation
	for(int directionalLight = 0; directionalLight < u_DirectionalLightCount; ++directionalLight)
	{
		color += BRDF_DirectionalLight(v, NoV, fi, lightIndex);
		++lightIndex;
	}
	
	float ambientOcclusion;
	if (u_IsSSAOEnabled)
	{
		ambientOcclusion = texture(u_SSAOSampler, fs_in.TexUV, 0).r;
	}
	else
	{
		ambientOcclusion = 1.f;
	}

	vec3 envFresnel = BRDF_Specular_F_Roughness(fi.mSpecularColor, fi.mRoughness * fi.mRoughness, NoV);

	if (u_HasEnvMap)
	{
		vec3 r = reflect(-v, fi.mNormal);
		vec3 envColor;
		
		if (fi.mEnvMapType == 0)
		{
			envColor = texture(u_EnvMap1Sampler, r).rgb;
		}
		else
		{
			envColor = texture(u_EnvMap2Sampler, r).rgb;
		}

		if(!u_IsEnvMapHDR)
		{
			envColor = pow(envColor, vec3(2.2));
		}

		return vec4(fi.mEmissive + ambientOcclusion * (color + envColor * envFresnel), 1.0f);
	}
	else
	{
		return vec4(fi.mEmissive + ambientOcclusion * color, 1.0f);
	}
}

void main(void)
{
	FragmentInfo fi;

	UnpackFromGBuffer(fi, fs_in.TexUV, fs_in.ViewRay, u_GBuffer1Sampler, u_DepthSampler);

	fi.mNormal = normalize(texture(u_NBufferSampler, fs_in.TexUV, 0).xyz);

	if (fi.mRendererId == SKYBOX_RENDERER_ID)
	{
		vFragColor = vec4(fi.mEmissive, 1);
	}
	else if(fi.mRendererId == DEBUG_RENDERER_ID)
	{
		vFragColor = vec4(fi.mBaseColor.xyz, 1);
	}
	else if(fi.mRendererId > 0)
	{
		vFragColor = BRDFLight(fi);
	}
	//else 
	//{
	//	vFragColor = vec4(0.f, 0.f, 0.f, 1.f);
	//}
}


//
// ---------------------------------------------------------------------------


