float NormalDistribution_BlinnPhong(float a, float NoH)
{
	float a2 = a * a;
    return (1 / (PI * a2)) * pow(NoH, 2 / a2 - 2);
}

float smithG_GGX(float NoV, float alphaG)
{
    float a = alphaG * alphaG;
    float b = NoV * NoV;
    return 1/(NoV + sqrt(a + b - a * b));
}


vec3 BRDF(in vec3 baseColor, in float metallic, in float roughness, in vec3 l, in vec3 v, in vec3 n, in vec3 lightColorIntensity, in float attenuation)
{
    vec3 albedoColor = baseColor - baseColor * metallic;
    // 0.03 default specular value for dielectric.
    vec3 specularColor = mix(vec3(0.03f), baseColor, metallic);
		
	vec3 h = normalize(l + v);

	float NoL = saturate(dot(n, l));
	float NoV = max(dot(n, v), 0.0001f);
	float NoH = saturate(dot(n, h));
	float LoH = saturate(dot(l, h));
	float LoH2 = LoH * LoH;

    // Diffuse fresnel - go from 1 at normal incidence to .5 at grazing
    // and mix in diffuse retro-reflection based on roughness
    float FL = BRDF_SchlickFresnel(NoL);
	float FV = BRDF_SchlickFresnel(NoV);
    float Fd90 = 0.5 + 2 * LoH2 * roughness;
    float Fd = mix(1, Fd90, FL) * mix(1, Fd90, FV);

	// specular
	float a = roughness * roughness;
	float Ds = NormalDistribution_BlinnPhong(a, NoH);
    float FH = BRDF_SchlickFresnel(LoH);
    vec3 Fs = mix(specularColor, vec3(1), FH);
    float roughg = sqrt(roughness * .5 + .5);
	float Gs = smithG_GGX(NoL, roughg) * smithG_GGX(NoV, roughg);

	vec3 brdf = ((1/PI) * Fd * albedoColor + Gs * Fs * Ds) * lightColorIntensity * NoL * attenuation;

	return brdf;
}
