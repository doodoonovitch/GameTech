#define NUMSTRIPS		36
#define MAX_NUMSTRIP	NUMSTRIPS
#define MAX_VERTICES	((MAX_NUMSTRIP + 1) * 3 * 2)

layout (point) in;
layout (triangle_strip, max_vertices = MAX_VERTICES) out;

uniform isamplerBuffer u_LightDescSampler;
uniform samplerBuffer u_LightDataSampler;
uniform int u_LightOffset;

in VS_OUT
{
	flat int InstanceId;
} gs_in[];

out GS_OUT
{
	vec4 Color;
} gs_out;

void main()
{  

	int index = gs_in[0].InstanceId + u_LightOffset;
	int lightDesc = texelFetch(u_LightDescSampler, index).x;

	int dataIndex = GetLightDataIndex(lightDesc);

	vec4 lightColorIntensity = texelFetch(u_LightDataSampler, dataIndex + LIGHT_COLOR_PROPERTY);
	vec4 lightColor = vec4(lightColorIntensity.xyz * lightColorIntensity.w, 1.f);
	vec4 attenuationCoef = texelFetch(u_lightDataSampler, dataIndex + SPOT_LIGHT_ATTENUATION_PROPERTY);

	float Imax = max(max(lightColor.x, lightColor.y), lightColor.z);
	float d = -attenuationCoef.y + sqrt(attenuationCoef.y * attenuationCoef.y - 4 * attenuationCoef.z * (attenuationCoef.x - Imax / 0.01)) / (2 * attenuationCoef.z);

	vec4 lightPosition = vec4(texelFetch(u_LightDataSampler, dataIndex + SPOT_LIGHT_WORLD_POSITION_PROPERTY).xyz, 1.f);
	vec4 lightDirection = d * vec4(texelFetch(u_LightDataSampler, dataIndex + SPOT_LIGHT_WORLD_DIRECTION_PROPERTY).xyz, 0.f);
	vec4 lightPosition2 = lightPosition + lightDirection;

	mat4 m = u_ProjMatrix * u_ViewMatrix;

	vec4 vertices[NUMSTRIPS + 1];

	for(int i = 0; i <= NUMSTRIPS; ++i)
	{
		float angle = mod((float)i * TWO_PI / (float)NUMSTRIPS, TWO_PI);
		vertices[i] = m * (vec4(0.5f * cos(angle), 0.f, 0.5f * sin(angle), 1.0f) + lightDirection);
	}

	lightPosition = m * lightPosition;
	lightPosition2 = m * lightPosition2;

	for(int i = 0; i < NUMSTRIPS; ++i )
	{	
		gl_Position = lightPosition;
		gs_out.Color = lightColor;
		EmitVertex();

		gl_Position = vertices[i];
		gs_out.Color = lightColor;
		EmitVertex();

		gl_Position = vertices[i + 1];
		gs_out.Color = lightColor;
		EmitVertex();
	}

	EndPrimitive();
}
