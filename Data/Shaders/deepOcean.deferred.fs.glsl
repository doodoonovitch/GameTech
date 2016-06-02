layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec3 outData;
layout(location = 2) out vec3 outNormal;

const int c_MaxWavesToSum = 4;

uniform vec3[c_MaxWavesToSum] u_Direction;
//uniform float[c_MaxWavesToSum] u_WaveLength;
uniform float[c_MaxWavesToSum] u_Frequency;
uniform float[c_MaxWavesToSum] u_Amplitude;
//uniform float[c_MaxWavesToSum] u_Velocity;
uniform float[c_MaxWavesToSum] u_Steepness;
uniform float[c_MaxWavesToSum] u_Phase;

uniform ivec2 u_MapSize;
uniform samplerCube u_SkyboxCubeMapSampler;
//uniform sampler2D u_textureSampler;


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
	//vec3 Normal;
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float SpecularPower;
};

void main()
{
	//vec3 normal = vec3(0, 1, 0);
	//vec3 normal = normalize(fs_in.Normal);
	
	vec3 normal = vec3(0);
	float t = u_TimeDeltaTime.x;
	
	for(int i = 0; i < c_MaxWavesToSum; ++i)
	{
		float dirPos = dot(u_Direction[i].xz, fs_in.TexUV.xy);
		float S = dirPos * u_Frequency[i] + t * u_Phase[i];

		float cosS = cos(S);
		float dhCommon = 0.5 * u_Steepness[i] * u_Frequency[i] * u_Amplitude[i] * cosS;

		if (u_Steepness[i] > 1)
		{
			float halfOfSinSplusOne = 0.5 * (1 + sin(S));
			float halfOfSinSplusOnePowSteepnessMinusOne = pow(halfOfSinSplusOne, u_Steepness[i] - 1);
			dhCommon *= halfOfSinSplusOnePowSteepnessMinusOne;
		}

		vec2 dH = vec2(u_Direction[i].x /* fs_in.TexUV.y*/ * dhCommon, u_Direction[i].z /* fs_in.TexUV.x*/ * dhCommon);
		normal = normal + vec3(dH.x, dH.y, 0);
	}		 

	normal = vec3(-normal.x, 1, -normal.y);
	normal = normalize(normal);
	
	Material mat;
	mat.SpecularColor = vec3(1);
	mat.SpecularPower = 64;
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
	outData = WriteOutData(DEEPOCEAN_RENDERER_ID , mat.DiffuseColor, mat.SpecularColor, int(mat.SpecularPower), vec3(0));
	outPosition = fs_in.ViewPosition;
	//outNormal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	outNormal = dqTransformNormal(normal, u_ViewDQ);
	//outNormal = (u_ViewMatrix * vec4(normal, 0)).xyz;
	//outNormal = vec3(0, 1, 0);
}
