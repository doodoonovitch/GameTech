layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


uniform sampler2D u_gBufferPosition;
uniform usampler2D u_gBufferData;
uniform sampler2D u_gBufferNormal;

uniform isamplerBuffer u_lightDescSampler;
uniform samplerBuffer u_lightDataSampler;


struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
	vec3 DiffuseMaterial;
	vec3 SpecularMaterial;
	vec3 EmissiveMaterial;
	float MaterialShininess;
	int RendererId;
};


// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi)
{
    vec4 encPositionNormal = texture(u_gBufferPosition, fs_in.TexUV, 0);
	fi.Position = encPositionNormal.xyz;
	fi.Normal = texture(u_gBufferNormal, fs_in.TexUV, 0).xyz;
	//fi.Normal.xy = unpackHalf2x16(floatBitsToUint(encPositionNormal.w));
	//fi.Normal.z = sqrt(1 - dot(fi.Normal.xy, fi.Normal.xy));
	//fi.Normal.xy = -unpackHalf2x16(floatBitsToUint(encPositionNormal.w));
	//fi.Normal.z = 1;

	vec4 temp;
	uvec3 data = texture(u_gBufferData, fs_in.TexUV, 0).xyz;
	fi.RendererId = int((data.x >> 24) & 255);

	uvec3 matD = uvec3(data.x & Mask_0x000000FF, (data.x >> 8) & Mask_0x000000FF, (data.x >> 16) & Mask_0x000000FF);
	fi.DiffuseMaterial = vec3(matD) / 255.f;

	////data.x = data.x & Mask_0x00FFFFFF;
	//temp = unpackUnorm4x8(data.x);
	//fi.DiffuseMaterial = temp.xyz;

	fi.MaterialShininess = pow(2, int((data.y >> 24) & 255));
	uvec3 matS = uvec3(data.x & Mask_0x000000FF, (data.x >> 8) & Mask_0x000000FF, (data.x >> 16) & Mask_0x000000FF);
	fi.SpecularMaterial = vec3(matS) / 255.f;

	////data.y = data.y & Mask_0x00FFFFFF;
	//temp = unpackUnorm4x8(data.y);
	//fi.SpecularMaterial = temp.xyz;

	temp = unpackUnorm4x8(data.z);
	fi.EmissiveMaterial = temp.xyz;

	//float normalZSign = temp.w;
	//fi.Normal.z = sqrt(1 - dot(fi.Normal.xy, fi.Normal.xy)) * normalZSign;
	//fi.Normal = normalize(fi.Normal);
}


//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Renderers
//
// ---------------------------------------------------------------------------

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

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.MaterialShininess) * attenuation * lightIntensity;	
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

			float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.MaterialShininess) * attenuation * lightIntensity;
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

		float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), fi.MaterialShininess) * lightIntensity;	
		specularColor += lightColor * specularFactor;
	}
	
	return vec4(fi.DiffuseMaterial * (ambientColor *.5 + diffuseColor) + fi.SpecularMaterial * specularColor + fi.EmissiveMaterial, 1);
}

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
	//else
	{
		vFragColor = ADSLight(fi);
	}
}

