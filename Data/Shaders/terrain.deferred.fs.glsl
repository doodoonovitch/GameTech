layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.08;
uniform float u_TriplanarBlendSharpness = 1.0;

uniform sampler2DArray u_HeightMap;
//uniform sampler2DArray u_DiffuseMap;
uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(int samplerIndex, vec3 p);
vec4 TexGetNormal(int samplerIndex, vec3 p);


in GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	vec3 Normal;
	vec3 ViewNormal;
	flat int Layer;
} fs_in;

struct Material
{
	vec3 DiffuseColor;
};

void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position);

void BlendMaterial(out Material blendedMat, Material lowSlopeMat, Material highSlopeMat, float blend)
{
	blendedMat.DiffuseColor = mix(lowSlopeMat.DiffuseColor, highSlopeMat.DiffuseColor, blend);
}


void main()
{
	vec3 normal = normalize(fs_in.Normal);

	vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float d = length(fs_in.WorldPosition);
	float k = floor(log(d / 7.5) / log(2));
	float texScale = pow(0.5, k - 2);
	vec3 uvs = texScale * fs_in.WorldPosition;
	//vec3 uvs = u_TexScale * fs_in.WorldPosition;

	Material mat;
	GetMaterial(mat, uvs, blendWeights, fs_in.Normal, fs_in.WorldPosition);

	outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, TERRAIN_RENDERER_ID / 255)), 0, 0);
	outPosition = vec4( fs_in.ViewPosition, uintBitsToFloat(packHalf2x16(fs_in.ViewNormal.xy)) );
}

//void GetMaterial(out BlendedMaterial blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position)
//{
//	vec3 diffuseY = texture(u_DiffuseMap, vec3(uvs.xz, 2)).xyz;
//	vec3 diffuseX = texture(u_DiffuseMap, vec3(uvs.zy, 2)).xyz;
//	vec3 diffuseZ = texture(u_DiffuseMap, vec3(uvs.xy, 2)).xyz;

//	blendedMat.DiffuseColor = (blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ);
//}
