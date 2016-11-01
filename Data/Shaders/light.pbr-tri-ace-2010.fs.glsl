vec3 GGX_BRDF(in vec3 diffuseMaterial, in vec3 specularMaterial, in float roughness, in vec3 l, in vec3 v, in vec3 n, in vec3 lightColorIntensity, in float attenuation)
{
	vec3 h = normalize(l + v);

	float NdotL = saturate(dot(n, l));
	float NdotV = max(dot(n, v), 0.0001f);
	float NdotH = saturate(dot(n, h));
	float LdotH = saturate(dot(l, h));

	//if(NdotL <= 0.0f)
	//	return vec3(0.0f);

	float m2 = roughness * roughness;
	float shininess = (2.0f / m2) - 2.0f;
	
	// Calculate the distribution term
	vec3 f = BRDF_Fresnel(specularMaterial, LdotH);
	vec3 Fspec = (0.0397436f * shininess + 0.0856832f) * f * pow(NdotH, shininess) / max(NdotL, NdotV);

	// Calculate the fresnel term for diffuse part
	vec3 f2 = BRDF_Fresnel(diffuseMaterial, NdotL);
	vec3 Fdiff = BRDF_BlinnPhongDiffuse(diffuseMaterial, f2);
		
	vec3 brdf = (Fdiff + Fspec) * lightColorIntensity * NdotL * attenuation;

	return brdf;
}
