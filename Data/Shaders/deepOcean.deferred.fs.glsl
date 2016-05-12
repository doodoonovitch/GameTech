layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec3 outData;
layout(location = 2) out vec3 outNormal;

const int c_MaxWavesToSum = 1;

uniform vec3[c_MaxWavesToSum] u_Direction;
uniform float[c_MaxWavesToSum] u_WaveLength;
uniform float[c_MaxWavesToSum] u_Frequency;
uniform float[c_MaxWavesToSum] u_Amplitude;
uniform float[c_MaxWavesToSum] u_Velocity;
uniform float[c_MaxWavesToSum] u_Steepness;
uniform float[c_MaxWavesToSum] u_Phase;

//uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];
//vec4 TexGet(int samplerIndex, vec3 p);


in GS_OUT
{
	vec3 Position;
	vec3 ViewPosition;
	vec3 Normal;
	//vec3 Tangent;
	flat DualQuat ViewModelDQ;
	//flat int MapIndex;
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
	vec3 normal = normalize(fs_in.Normal);
	//vec3 dxH = dFdx(fs_in.Position);
	//vec3 dyH = dFdy(fs_in.Position);
	//vec3 normal = cross(dyH, dxH);

	//vec2 dH = vec2(0);
	//float t = float(u_TimeDeltaTime.x);
	
	//for(int i = 0; i < c_MaxWavesToSum; ++i)
	//{
	//	float dirPos = dot(u_Direction[i].xz, fs_in.Position.xz);
	//	float S = dirPos * u_Frequency[i] + t * u_Phase[i];

	//	float cosS = cos(S);
	//	float dhCommon = 0.5 * u_Steepness[i] * u_Frequency[i] * u_Amplitude[i] * cosS;

	//	if (u_Steepness[i] > 1)
	//	{
	//		float halfOfSinSplusOne = 0.5 * (1 + sin(S));
	//		float halfOfSinSplusOnePowSteepnessMinusOne = pow(halfOfSinSplusOne, u_Steepness[i] - 1);
	//		dhCommon = dhCommon * halfOfSinSplusOnePowSteepnessMinusOne;
	//	}

	//	dH.x = dH.x + (u_Direction[i].x * dhCommon);
	//	dH.y = dH.y + (u_Direction[i].z * dhCommon);
	//}		 
	////vec3 normal = normalize(vec3(-dH.x, 1, -dH.y));
	//vec3 B = normalize(vec3(1, dH.x, 0));
	//vec3 T = normalize(vec3(0, dH.y, -1));
	//vec3 normal = cross(T, B);


	Material mat;
	mat.SpecularColor = vec3(0);
	mat.SpecularPower = 64;
	mat.DiffuseColor = vec3(0.f, 0.f, 1.f);

	//outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, 0)), packUnorm4x8(vec4(mat.SpecularColor, mat.SpecularPower / 255)), 0);
	//outData.x = outData.x | (DEEPOCEAN_RENDERER_ID << 24);
	outData = WriteOutData(DEEPOCEAN_RENDERER_ID , mat.DiffuseColor, mat.SpecularColor, int(mat.SpecularPower), vec3(0));
	outPosition = fs_in.ViewPosition;
	outNormal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	//outNormal = normal;
}
