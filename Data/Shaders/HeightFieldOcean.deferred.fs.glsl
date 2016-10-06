layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_textureSampler;

#ifdef PER_VERTEX_NORMAL

in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	vec3 Normal;
	flat int MapIndex;
} fs_in;

#else

uniform sampler2D u_HeightMapSampler;

const float u_dUV = 1/512;
const vec2 u_Step = vec2(1.0, 0.0);

in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	flat int MapIndex;
} fs_in;

#endif

struct Material
{
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Roughness;
};



void main()
{
	vec2 texUV = fs_in.TexUV;

#ifdef PER_VERTEX_NORMAL
	vec3 normal = normalize(fs_in.Normal);
#else
	float nX = textureOffset(u_HeightMapSampler, texUV, ivec2(-1, 0)).r - textureOffset(u_HeightMapSampler, texUV, ivec2(1, 0)).r;
	float nZ = textureOffset(u_HeightMapSampler, texUV, ivec2(0, -1)).r - textureOffset(u_HeightMapSampler, texUV, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX, 2, nZ));
#endif

	Material mat;
	mat.SpecularColor = vec3(0.2);
	mat.Roughness = .1;

	vec3 refractColor = texture(u_textureSampler, texUV).rgb;
	//vec3 viewDir = normalize(fs_in.Position - u_ViewPosition.xyz);
	//vec3 R = reflect(viewDir, normal);
	//vec3 reflectColor = texture(u_SkyboxCubeMapSampler, R).rgb;
	mat.SpecularColor = refractColor;

	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, DEEPOCEAN_RENDERER_ID , mat.DiffuseColor, mat.SpecularColor, mat.Roughness, vec3(0));
	outPosition = fs_in.ViewPosition;
	//outNormal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	outNormal = dqTransformNormal(normal, u_ViewDQ);
	//outNormal = (u_ViewMatrix * vec4(normal, 0)).xyz;
	//outNormal = vec3(0, 1, 0);
}
