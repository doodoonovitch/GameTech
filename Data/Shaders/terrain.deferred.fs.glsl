layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outUI32Buffer1;

layout (std430, binding = 2) buffer Materials
{
	ModelMaterial u_Materials[];
};

uniform vec3 u_Scale;
uniform float u_TriplanarBlendSharpness = 2.0;

//uniform sampler2DArray u_HeightMap;
uniform sampler2DArray u_BlendMap;
uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

in GS_OUT
{
	vec4 Blend;
	vec3 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MapIndex;
} fs_in;

struct Material
{
	vec3 BaseColor;
	float Metallic;
	float Roughness;
	float Height;
};

// Project the surface gradient (dhdx, dhdy) onto the surface (n, dpdx, dpdy)
vec3 CalculateSurfaceGradient(vec3 n, vec3 dpdx, vec3 dpdy, float dhdx, float dhdy)
{
    vec3 r1 = cross(dpdy, n);
    vec3 r2 = cross(n, dpdx);

    return (r1 * dhdx + r2 * dhdy) / dot(dpdx, r1);
}
 
// Calculate the surface normal using screen-space partial derivatives of the height field
vec3 PerturbNormal(vec3 position, vec3 normal, float height)
{
    vec3 dpdx = dFdx(position);
    vec3 dpdy = dFdy(position);
 
    float dhdx = dFdx(height);
    float dhdy = dFdy(height);
 
	return normalize(normal - CalculateSurfaceGradient(normal, dpdx, dpdy, dhdx, dhdy));
}


//void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position);

void BlendMaterial(out Material blendedMat, Material lowSlopeMat, Material highSlopeMat, float blend)
{
	blendedMat.BaseColor = mix(lowSlopeMat.BaseColor, highSlopeMat.BaseColor, blend);
	blendedMat.Metallic = mix(lowSlopeMat.Metallic, highSlopeMat.Metallic, blend);
	blendedMat.Roughness = mix(lowSlopeMat.Roughness, highSlopeMat.Roughness, blend);
}

void main()
{
	MaterialData matData;
	matData.mRendererId = TERRAIN_RENDERER_ID;

	vec3 normal = normalize(fs_in.Normal);

	//vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	//blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float d = length(fs_in.TexUV);
	float k = floor(log(d / 7.5) / log(2));
	float texScale = pow(0.5, k - 2);
	vec2 uvs = texScale * fs_in.TexUV;

	float height = texture(u_textureSampler[2], vec3(uvs, 0)).x;
	vec3 color = texture(u_textureSampler[0], vec3(uvs, 0)).x;
	
	normal = PerturbNormal(fs_in.Position, normal, mat.Height);

	matData.mBaseColor = mat.BaseColor;
	matData.mRoughness = mat.Roughness;
	matData.mMetallic =  mat.Metallic;
	matData.mPorosity =  0;
	matData.mEmissive =  vec3(0);

	outNormal = normal;
	EncodeMaterialData(outUI32Buffer1, matData);
}
