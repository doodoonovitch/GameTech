layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

const int c_MaxWavesToSum = 5;

uniform vec3[c_MaxWavesToSum] u_Direction;
//uniform float[c_MaxWavesToSum] u_WaveLength;
uniform float[c_MaxWavesToSum] u_Frequency;
uniform float[c_MaxWavesToSum] u_Amplitude;
//uniform float[c_MaxWavesToSum] u_Velocity;
uniform float[c_MaxWavesToSum] u_Steepness;
uniform float[c_MaxWavesToSum] u_Phase;

uniform ivec2 u_MapSize;
uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_noiseSampler;
uniform sampler2D u_textureSampler;


in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	vec3 Normal;
	//vec3 Tangent;
	flat int MapIndex;
} fs_in;


struct Material
{
	vec3 Normal;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float SpecularPower;
};

float F(vec3 p)
{
	return texture(u_noiseSampler, vec2(texture(u_noiseSampler, p.xy).x, p.z)).x;
}

void main()
{
	//vec3 normal = normalize(fs_in.Normal);
	
	vec3 normal = vec3(0);
	float t = u_TimeDeltaTime.x;
	
	for(int i = 0; i < c_MaxWavesToSum; ++i)
	{
		float dirPos = dot(u_Direction[i].xz, fs_in.TexUV.xy);
		float S = dirPos * u_Frequency[i] + t * u_Phase[i];

		float cosS = cos(S);
		float dhCommon = 0.5 * u_Steepness[i] * u_Frequency[i] * u_Amplitude[i] * cosS;

		if (u_Steepness[i] != 1)
		{
			float halfOfSinSplusOne = 0.5 * (1 + sin(S));
			float halfOfSinSplusOnePowSteepnessMinusOne = pow(halfOfSinSplusOne, u_Steepness[i] - 1);
			dhCommon *= halfOfSinSplusOnePowSteepnessMinusOne;
		}

		vec2 dH = vec2(u_Direction[i].x * dhCommon, u_Direction[i].z * dhCommon);
		normal = normal + vec3(-dH.x, 1, -dH.y);
	}		 
	normal = normalize(normal);

	////vec3 bumpMapNormal = texture(u_noiseSampler, fs_in.TexUV * t + vec2(0.1, 0.15) * t).xyz; // + texture(u_noiseSampler, fs_in.TexUV + vec2(0.01, 0.02) * t).xyz;
	//vec3 bumpMapNormal = texture(u_noiseSampler, fs_in.TexUV * 5 + vec2(0.1, 0.15) * t).xyz;
	//bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);	
	//normal = normalize(normal + bumpMapNormal);
	
	Material mat;
	mat.SpecularColor = vec3(0.2);
	mat.SpecularPower = .2;
	//vec2 texUV = fs_in.TexUV * 64.f;
	//vec2 texUV = fs_in.TexUV;
	//mat.DiffuseColor = texture(u_textureSampler, texUV).rgb;
	mat.DiffuseColor = vec3(0, 0.2, 0.8);
	
	//vec3 viewDir = fs_in.Position - u_ViewPosition.xyz;
	//viewDir = normalize(viewDir);
	//vec3 R = reflect(viewDir, normal);
	//mat.DiffuseColor = texture(u_SkyboxCubeMapSampler, R).xyz;

	//outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, 0)), packUnorm4x8(vec4(mat.SpecularColor, mat.SpecularPower / 255)), 0);
	//outData.x = outData.x | (DEEPOCEAN_RENDERER_ID << 24);
	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, DEEPOCEAN_RENDERER_ID , mat.DiffuseColor, mat.SpecularColor, mat.SpecularPower, vec3(0));
	outPosition = fs_in.ViewPosition;
	//outNormal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	outNormal = dqTransformNormal(normal, u_ViewDQ);
	//outNormal = (u_ViewMatrix * vec4(normal, 0)).xyz;
	//outNormal = vec3(0, 1, 0);
}
