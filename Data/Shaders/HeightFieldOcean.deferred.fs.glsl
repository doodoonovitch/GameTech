layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outRoughnessAndOthers;

uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_textureSampler;

#ifdef PER_PIXEL_NORMAL

uniform sampler2D u_NormalMapSampler;
uniform float u_MaxAmplitude;

in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	flat int MapIndex;
} fs_in;


#else


in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	vec3 Normal;
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

#ifdef PER_PIXEL_NORMAL
	float nX = textureOffset(u_NormalMapSampler, texUV, ivec2(-1, 0)).r - textureOffset(u_NormalMapSampler, texUV, ivec2(1, 0)).r;
	float nZ = textureOffset(u_NormalMapSampler, texUV, ivec2(0, -1)).r - textureOffset(u_NormalMapSampler, texUV, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX, 2, nZ));
#else
	vec3 normal = normalize(fs_in.Normal);
#endif

	Material mat;
	mat.SpecularColor = vec3(0.2);
	mat.Roughness = .2;

	vec3 refractColor = texture(u_textureSampler, texUV).rgb;
	//vec3 viewDir = normalize(fs_in.Position - u_ViewPosition.xyz);
	//vec3 R = reflect(viewDir, normal);
	//vec3 reflectColor = texture(u_SkyboxCubeMapSampler, R).rgb;
	//mat.SpecularColor = refractColor;
	mat.DiffuseColor = refractColor;

	normal = dqTransformNormal(normal, u_ViewDQ);

	WriteOutData(outAlbedoAndStatus, outRoughnessAndOthers, DEEPOCEAN_RENDERER_ID , mat.DiffuseColor, 1.0f, mat.SpecularPower, 0.f);
	outNormal = vec3(normal.xyz);
}
