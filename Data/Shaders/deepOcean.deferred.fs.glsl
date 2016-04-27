layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec3 outData;
layout(location = 2) out vec3 outNormal;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.1;
//uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

//vec4 TexGet(int samplerIndex, vec3 p);

in GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	vec3 WorldNormal;
	vec3 ViewNormal;
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
	vec3 normal = normalize(fs_in.ViewNormal);
	Material mat;
	mat.SpecularColor = vec3(0);
	mat.SpecularPower = 1;
	vec3 x = fs_in.WorldPosition + vec3(u_TimeDeltaTime.xxx);
	//mat.DiffuseColor = vec3(0.5 * (sin(x) + vec3(1)));
	mat.DiffuseColor = vec3(0.f, 0.f, 1.f);

	outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, DEEPOCEAN_RENDERER_ID / 255)), packUnorm4x8(vec4(mat.SpecularColor, mat.SpecularPower / 255)), 0);
	outPosition = fs_in.ViewPosition;
	outNormal = normal;
}
