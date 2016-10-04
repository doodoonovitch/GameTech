layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

uniform samplerCube u_SkyboxCubeMapSampler;
uniform sampler2D u_textureSampler;


in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	vec3 Normal;
	flat int MapIndex;
} fs_in;


struct Material
{
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float Roughness;
};

void main()
{
	vec3 normal = normalize(fs_in.Normal);

	Material mat;
	mat.SpecularColor = vec3(0.2);
	mat.Roughness = .1;

	vec2 texUV = fs_in.TexUV;
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
