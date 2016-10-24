layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} fs_in;


//uniform sampler2D u_gBufferPosition;
uniform sampler2D u_gBufferNormal;
uniform usampler2D u_gBufferAlbedoAndStatus;
uniform sampler2D u_gBufferSpecularRoughness;
uniform sampler2D u_gBufferEmissive;
uniform sampler2D u_gDepthMap;

uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;


struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec3 DiffuseMaterial;
	vec3 SpecularMaterial;
	vec3 EmissiveMaterial;
	float Roughness;
	//float GlossPower;
	int RendererId;
};



// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi);


//
// ---------------------------------------------------------------------------

/*
// ---------------------------------------------------------------------------
// BRDF Dpl(n, h)
//	alpha = pow(8192, glossPower)
// ---------------------------------------------------------------------------
float BRDF_Dpl(in float NdotH, in float alpha)
{
	return ((alpha + 2) / 8) * pow(NdotH, alpha);
}


//
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
// BRDF F(rf0, l, h)
//
// ---------------------------------------------------------------------------
vec3 BRDF_F(in vec3 rf0, in float HdotV)
{
	return rf0 + (vec3(1) - rf0) * pow(1 - HdotV, 5);
}


//
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
// BRDF V(l, v, n)
//
// ---------------------------------------------------------------------------
float BRDF_V(in float NdotL, in float NdotV, in float alpha)
{
	float k = 2 / sqrt(PI * (alpha + 2));
	float oneMinusK = 1 - k;
	return 1 / ((NdotL * oneMinusK + k) * (NdotV * oneMinusK + k));
}


//
// ---------------------------------------------------------------------------




// ---------------------------------------------------------------------------
// BRDF(rf0, l, h, v, n, glossPower)
//
// ---------------------------------------------------------------------------
vec3 BRDF_Specular(in vec3 rf0, in float NdotL, in float NdotV, in float NdotH, in float HdotV, in float glossPower)
{
	float alpha = pow(2, glossPower);
	return BRDF_Dpl(NdotH, alpha) * BRDF_V(NdotL, NdotV, alpha) * BRDF_F(rf0, HdotV);
}
*/

// ---------------------------------------------------------------------------
// BRDF_LambertDiffuse(Kd)
//
// ---------------------------------------------------------------------------
vec3 BRDF_LambertDiffuse(in vec3 Kd)
{
	return Kd / PI;
}

// ---------------------------------------------------------------------------
// BRDF_Diffuse(Kd, LdotH)
//
// ---------------------------------------------------------------------------
vec3 BRDF_Diffuse(in vec3 Kd, in vec3 fresnel)
{
	return (vec3(1) -  fresnel) * Kd / PI;
}



// ===============================================================================
// Calculates the Fresnel factor using Schlick's approximation
//
//		float LdotH = saturate(dot(l, h));
// ===============================================================================
vec3 BRDF_Fresnel(in vec3 specAlbedo, in float LdotH)
{
	return specAlbedo + (1.0f - specAlbedo) * pow((1.0f - LdotH), 5.0f);
}
// ===============================================================================
// Helper for computing the GGX visibility term
// ===============================================================================
float GGX_V1(in float m2, in float NdotX)
{
	return 1.0f / (NdotX + sqrt(m2 + (1 - m2) * NdotX * NdotX));
}
// ===============================================================================
// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. m is roughness, n is the surface normal,
// h is the half vector, l is the direction to the light source, and specAlbedo is
// the RGB specular albedo
//
//	float NdotL = saturate(dot(n, l));
//	float NdotH = saturate(dot(n, h));
//	float NdotV = max(dot(n, v), 0.0001f);
//	float LdotH = saturate(dot(l, h));
// ===============================================================================
vec3 GGX_Specular(in vec3 specAlbedo, in float m, in float NdotL, in float NdotH, in float NdotV, in float LdotH)
{
	if(NdotL <= 0.0f)
		return vec3(0.0f);

	float NdotH2 = NdotH * NdotH;
	float m2 = m * m;
	
	// Calculate the distribution term
	float d = m2 / (PI * pow(NdotH2 * (m2 - 1) + 1, 2.0f));

	// Calculate the matching visibility term
	float v1i = GGX_V1(m2, NdotL);
	float v1o = GGX_V1(m2, NdotV);
	float vis = v1i * v1o;

	// Calculate the fresnel term
	vec3 f = BRDF_Fresnel(specAlbedo, LdotH);
	// Put it all together
	return d * f * vis;
}
//
// ---------------------------------------------------------------------------

vec3 GGX_BRDF(in vec3 diffuseMaterial, in vec3 specularMaterial, in float roughness, 
	in float NdotL, in float NdotH, in float NdotV, in float LdotH, 
	in vec3 lightColorIntensity, in float attenuation)
{
	if(NdotL <= 0.0f)
		return vec3(0.0f);

	float NdotH2 = NdotH * NdotH;
	float m2 = roughness * roughness;
	
	// Calculate the distribution term
	float d = m2 / (PI * pow(NdotH2 * (m2 - 1) + 1, 2.0f));

	// Calculate the matching visibility term
	float v1i = GGX_V1(m2, NdotL);
	float v1o = GGX_V1(m2, NdotV);
	float vis = v1i * v1o;

	// Calculate the fresnel term
	vec3 f = BRDF_Fresnel(specularMaterial, LdotH);
	//vec3 f2 = BRDF_Fresnel(diffuseMaterial, LdotH);

	//vec3 brdf = BRDF_Diffuse(diffuseMaterial, f2) 
	vec3 brdf = BRDF_LambertDiffuse(diffuseMaterial)
		+ d * f * vis;

	brdf = brdf * lightColorIntensity * NdotL * attenuation;

	return brdf;
}



float AngleAttenuation(in vec3 lightDirection, in vec3 spotDirection, in float innerConeCos, in float outerConeCos)
{
	float theta = dot(lightDirection, spotDirection); 
	float epsilon = max(innerConeCos - outerConeCos, 0.001);
	float spotIntensity = clamp((theta - outerConeCos) / epsilon, 0.0, 1.0);
	return spotIntensity;
}

float DistAttenuation(in vec3 unnormalizedLightVector, in float lightRadius)
{
	//float lightDistance = length(unnormalizedLightVector);
	//float fact1 = saturate(1 - pow(lightDistance / lightRadius, 4));
	//float fact2 = 1 / (lightDistance * lightDistance + 1);
	//float attenuation = fact1 * fact1 * fact2;

	float dist2 = dot(unnormalizedLightVector, unnormalizedLightVector);
	float invDist2 = 1 / max(dist2, 0.01 * 0.01);
	int N = 4;
	float lightRadiusN = pow(lightRadius, N);
	float distN = pow(dist2, N /2);
	float fact2 = saturate(1 - (distN / lightRadiusN));
	float attenuation = invDist2 * fact2 * fact2;
	return attenuation;
}

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

vec3 BRDF_PointLight(in vec3 v, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + POINT_LIGHT_POSITION_PROPERTY);

	vec3 unnormalizedLightVector = lightPosition.xyz - fi.Position.xyz;
	vec3 l = normalize(unnormalizedLightVector);
	
	float attenuation = DistAttenuation(unnormalizedLightVector, lightRadius);

	vec3 h = normalize(l + v);

	float NdotL = saturate(dot(fi.Normal, l));
	float NdotV = max(dot(fi.Normal, v), 0.0001f);
	float NdotH = saturate(dot(fi.Normal, h));
	float LdotH = saturate(dot(l, h));

	return GGX_BRDF(fi.DiffuseMaterial, fi.SpecularMaterial, fi.Roughness, NdotL, NdotH, NdotV, LdotH, lightColorIntensity, attenuation);
}

vec3 BRDF_SpotLight(in vec3 v, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_POSITION_PROPERTY);
	float outerConeCos = lightPosition.w;
	lightPosition.w = 1.0;

	vec3 unnormalizedLightVector = lightPosition.xyz - fi.Position.xyz;
	vec3 l = normalize(unnormalizedLightVector);

	vec4 spotData = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_DIRECTION_PROPERTY);
	vec3 spotDirection = normalize(-spotData.xyz);
	float innerConeCos = spotData.w;
	
	float attenuation = DistAttenuation(unnormalizedLightVector, lightRadius);
	attenuation *= AngleAttenuation(l, spotDirection, innerConeCos, outerConeCos);

	vec3 h = normalize(l + v);

	float NdotL = saturate(dot(fi.Normal, l));
	float NdotV = max(dot(fi.Normal, v), 0.0001f);
	float NdotH = saturate(dot(fi.Normal, h));
	float LdotH = saturate(dot(l, h));

	return GGX_BRDF(fi.DiffuseMaterial, fi.SpecularMaterial, fi.Roughness, NdotL, NdotH, NdotV, LdotH, lightColorIntensity, attenuation);
}

vec3 BRDF_DirectionalLight(in vec3 v, in FragmentInfo fi, in int lightIndex)
{
	int dataIndex;
	vec3 lightColorIntensity;
	float lightRadius;
	GetCommonLightProperties(lightIndex, dataIndex, lightColorIntensity, lightRadius);

	vec3 l = texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;

	vec3 h = normalize(l + v);

	float NdotL = saturate(dot(fi.Normal, l));
	float NdotV = max(dot(fi.Normal, v), 0.0001f);
	float NdotH = saturate(dot(fi.Normal, h));
	float LdotH = saturate(dot(l, h));

	return GGX_BRDF(fi.DiffuseMaterial, fi.SpecularMaterial, fi.Roughness, NdotL, NdotH, NdotV, LdotH, lightColorIntensity, 1.0);
}


vec4 BRDFLight(FragmentInfo fi)
{
	vec3 v = normalize(-fi.Position);
	int lightIndex = 1;

	vec3 color = vec3(0);

	// point light evaluation
	for(int pointLight = 0; pointLight < u_PointLightCount; ++pointLight)
	{
		color += BRDF_PointLight(v, fi, lightIndex);
		++lightIndex;
	}

	// spot light evaluation
	for(int spotLight = 0; spotLight < u_SpotLightCount; ++spotLight)
	{
		color += BRDF_SpotLight(v, fi, lightIndex);
		++lightIndex;
	}

	// directional light evaluation
	for(int directionalLight = 0; directionalLight < u_DirectionalLightCount; ++directionalLight)
	{
		color += BRDF_DirectionalLight(v, fi, lightIndex);
		++lightIndex;
	}
	
	return vec4(color, 1);
}

/*
vec4 ADSLight(FragmentInfo fi)
{
	vec3 ambientColor = u_AmbientLight.xyz;
	vec3 diffuseColor = vec3(0, 0, 0);
	vec3 specularColor = vec3(0, 0, 0);

	vec3 viewDirection = normalize(-fi.Position);

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

		vec3 lightDirection = lightPosition.xyz - fi.Position.xyz;
		float lightDistance = length(lightDirection);
		lightDirection = lightDirection / lightDistance;
		//lightDirection = normalize(lightDirection);

		float attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

		vec3 reflectionDirection = reflect(-lightDirection, fi.Normal);

		float ambientFactor = attenuation * lightIntensity;
		ambientColor += lightColor.xyz * ambientFactor;

		float diffuseFactor = max(0, dot(fi.Normal, lightDirection)) * attenuation * lightIntensity;
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.GlossPower) * attenuation * lightIntensity;	
		specularColor += lightColor * specularFactor;
	}
	
	// spot light evaluation
	for(int spotLight = 0; spotLight < u_SpotLightCount; ++spotLight)
	{
		int lightDesc = texelFetch(u_lightDescSampler, lightIndex).x;
		++lightIndex;

		int lightType = GetLightType(lightDesc);
		int dataIndex = GetLightDataIndex(lightDesc);

		vec4 lightPosition = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_POSITION_PROPERTY);

		vec3 lightDirection = lightPosition.xyz - fi.Position.xyz;
		float lightDistance = length(lightDirection);
		lightDirection = lightDirection / lightDistance;

		float LdotN = dot(fi.Normal, lightDirection);

		//if ((LdotN - 0.000001f) > 0)
		{
			vec4 lightColorIntensity = texelFetch(u_lightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
			vec3 lightColor = lightColorIntensity.xyz;
			float lightIntensity = lightColorIntensity.w;

			vec4 spotData = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_DIRECTION_PROPERTY);
			vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_ATTENUATION_PROPERTY);
			vec3 spotDirection = normalize(-spotData.xyz);
			float innerConeCos = spotData.w;
			float outerConeCos = attenuationCoef.w;

			float attenuation = 1.0 / (attenuationCoef.x + attenuationCoef.y * lightDistance + attenuationCoef.z * lightDistance * lightDistance);

			vec3 reflectionDirection = reflect(-lightDirection, fi.Normal);

			float theta = dot(lightDirection, spotDirection); 
			float epsilon = (innerConeCos - outerConeCos);
			float spotIntensity = clamp((theta - outerConeCos) / epsilon, 0.0, 1.0);

			attenuation = attenuation * spotIntensity;

			float ambientFactor = attenuation * lightIntensity;
			ambientColor += lightColor * ambientFactor;

			float diffuseFactor = max(0, LdotN) * attenuation * lightIntensity;
			diffuseColor += lightColor * diffuseFactor; 

			float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.GlossPower) * attenuation * lightIntensity;
			specularColor += lightColor * specularFactor;
		}
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

		lightDirection = texelFetch(u_lightDataSampler, dataIndex + DIRECTIONAL_LIGHT_DIRECTION_PROPERTY).xyz;
		reflectionDirection = reflect(lightDirection, fi.Normal);

		lightDirection = -lightDirection;
		float ambientFactor = lightIntensity;
		ambientColor += lightColor * ambientFactor;

		float diffuseFactor = max(0, dot(fi.Normal, lightDirection)) * lightIntensity;
		diffuseColor += lightColor * diffuseFactor; 

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.GlossPower) * lightIntensity;	
		specularColor += lightColor * specularFactor;
	}
	
	return vec4(fi.DiffuseMaterial * (ambientColor *.5 + diffuseColor) + fi.SpecularMaterial * specularColor + fi.EmissiveMaterial, 1);
}
*/

//vec4 GridRenderer(vec3 Position, int MaterialIndex)
//{
//    uvec2 data = texture(u_gBufferData, fs_in.TexUV, 0).xy;
//    vec2 texUV = unpackHalf2x16(data.x);

//	return vec4(texUV.xy, 1, 1);
//}

//vec4 ColorRenderer(vec3 Position, int MaterialIndex)
//{
//    uvec2 data = texture(u_gBufferData, fs_in.TexUV, 0).xy;
//    vec4 color = vec4(unpackHalf2x16(data.x), unpackHalf2x16(data.y));

//	return color;
//}


//
// ---------------------------------------------------------------------------


void main(void)
{
	FragmentInfo fi;

	UnpackFromGBuffer(fi);
	
	//if (fi.RendererId == DEEPOCEAN_RENDERER_ID)
	//{
	//	vFragColor = vec4((fi.Normal + 1) * 0.5, 1);
	//}
	if (fi.RendererId == SKYBOX_RENDERER_ID)
	{
		vFragColor = vec4(fi.SpecularMaterial, 1);
	}
	else
	{
		//vFragColor = ADSLight(fi);
		vFragColor = BRDFLight(fi);
	}
}

