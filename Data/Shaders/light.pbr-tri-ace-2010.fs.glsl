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

	float m2 = max(0.001, roughness * roughness);
	float shininess = (2.0f / m2) - 2.0f;
	
	// Calculate the distribution term
	vec3 f = BRDF_Fresnel(specularColor, LoH);
	vec3 Fspec = (0.0397436f * shininess + 0.0856832f) * f * pow(NoH, shininess) / max(NoL, NoV);

	// Calculate the fresnel term for diffuse part
	vec3 f2 = BRDF_Fresnel(albedoColor, NoL);
	vec3 Fdiff = 1 / PI * f2;
	//vec3 Fdiff = BRDF_LambertDiffuse(albedoColor);
		
	vec3 brdf = (Fdiff * (vec3(1) - Fspec) + Fspec) * lightColorIntensity * NoL * attenuation;

	return brdf;
}
