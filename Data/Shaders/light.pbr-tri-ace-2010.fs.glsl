vec3 BRDF(in vec3 diffuseColor, in vec3 specularColor, in float roughness, in vec3 l, in vec3 v, in vec3 n, in float NoV, in vec3 lightColorIntensity, in float attenuation)
{	
	vec3 h = normalize(l + v);

	float NoL = saturate(dot(n, l));
	float NoH = saturate(dot(n, h));
	float LoH = saturate(dot(l, h));

	float m2 = max(0.0001, roughness * roughness);
	float shininess = (2.0f / m2) - 2.0f;
	
	// Calculate the distribution term
	vec3 f = BRDF_SchlickFresnel(specularColor, LoH);
	vec3 Fspec = (0.0397436f * shininess + 0.0856832f) * f * pow(NoH, shininess) / max(NoL, NoV);

	// Calculate the fresnel term for diffuse part
	//vec3 f2 = BRDF_SchlickFresnel(albedoColor, NoL);
	//vec3 Fdiff = 1 / PI * f2;
	vec3 Fdiff = BRDF_LambertDiffuse(diffuseColor);
		
	vec3 brdf = (Fdiff + Fspec) * lightColorIntensity * NoL * attenuation;

	return brdf;
}
