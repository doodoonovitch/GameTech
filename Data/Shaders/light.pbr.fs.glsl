
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
	vec3 f = BRDF_SchlickFresnel(specAlbedo, LdotH);
	// Put it all together
	return d * f * vis;
}
//
// ---------------------------------------------------------------------------

vec3 GGX_BRDF(in vec3 diffuseMaterial, in vec3 specularMaterial, in float roughness, in vec3 l, in vec3 v, in vec3 n, in vec3 lightColorIntensity, in float attenuation)
{
	vec3 h = normalize(l + v);

	float NdotL = saturate(dot(n, l));
	float NdotV = max(dot(n, v), 0.0001f);
	float NdotH = saturate(dot(n, h));
	float LdotH = saturate(dot(l, h));

	if(NdotL <= 0.0f)
		return vec3(0.0f);

	float NdotH2 = NdotH * NdotH;
	float m2 = roughness * roughness;
	
	// Calculate the matching visibility term
	float v1i = GGX_V1(m2, NdotL);
	float v1o = GGX_V1(m2, NdotV);
	float vis = v1i * v1o;

	// Calculate the distribution term
	vec3 f = BRDF_SchlickFresnel(specularMaterial, LdotH);
	float d = m2 / (PI * pow(NdotH2 * (m2 - 1) + 1, 2.0f));
	vec3 Fspec = d * f * vis;

	// Calculate the fresnel term
	vec3 f2 = BRDF_SchlickFresnel(diffuseMaterial, NdotL);
	vec3 Fdiff = BRDF_BlinnPhongDiffuse(diffuseMaterial, f2);
		
	vec3 brdf = (Fdiff + Fspec) * lightColorIntensity * NdotL * attenuation;

	return brdf;
}


