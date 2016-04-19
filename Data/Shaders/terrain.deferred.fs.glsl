layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.05;
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
	int DiffuseSamplerIndex;
	int DiffuseLayerIndex;

	vec3 DiffuseColor2;
	int DiffuseSamplerIndex2;
	int DiffuseLayerIndex2;

	float DiffuseBlend;
};

struct BlendedMaterial
{
	vec3 DiffuseColor;
};

void GetMaterial(out BlendedMaterial blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position);

vec3 GetTexture(int samplerIndex, int layerIndex, vec3 uvs, vec3 blendWeights)
{
	vec3 diffuseY = TexGet(samplerIndex, vec3(uvs.xz, layerIndex)).xyz;
	vec3 diffuseX = TexGet(samplerIndex, vec3(uvs.zy, layerIndex)).xyz;
	vec3 diffuseZ = TexGet(samplerIndex, vec3(uvs.xy, layerIndex)).xyz;

	return ((blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ));
}

void BlendMaterials(out BlendedMaterial blendedMat, Material mat, vec3 uvs, vec3 blendWeights)
{
	if (mat.DiffuseSamplerIndex >= 0)
	{
		blendedMat.DiffuseColor = mat.DiffuseColor * GetTexture(mat.DiffuseSamplerIndex, mat.DiffuseLayerIndex, uvs, blendWeights);
	}

	if (mat.DiffuseSamplerIndex2 >= 0 && mat.DiffuseBlend > 0)
	{
		vec3 color2 = mat.DiffuseColor2 * GetTexture(mat.DiffuseSamplerIndex2, mat.DiffuseLayerIndex2, uvs, blendWeights);
		blendedMat.DiffuseColor = mix(blendedMat.DiffuseColor, color2, mat.DiffuseBlend);
	}	
}

void main()
{
	vec3 normal = normalize(fs_in.Normal);

	vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float d = length(fs_in.WorldPosition);
	float k = floor(log(d / 7.5) / log(2));
	float texScale = pow(0.5, k - 2);
	vec3 uvs = u_TexScale * texScale * fs_in.WorldPosition;
	//vec3 uvs = u_TexScale * fs_in.WorldPosition;

	BlendedMaterial mat;
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
