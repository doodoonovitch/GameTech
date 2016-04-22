layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.1;
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
	//flat int MapIndex;
} fs_in;

struct Material
{
	vec3 DiffuseColor;
	vec3 SpecularColor;
	float SpecularPower;
	float Height;
};

// Project the surface gradient (dhdx, dhdy) onto the surface (n, dpdx, dpdy)
vec3 CalculateSurfaceGradient(vec3 n, vec3 dpdx, vec3 dpdy, float dhdx, float dhdy)
{
    vec3 r1 = cross(dpdy, n);
    vec3 r2 = cross(n, dpdx);
 
    return (r1 * dhdx + r2 * dhdy) / dot(dpdx, r1);
}
 
// Move the normal away from the surface normal in the opposite surface gradient direction
vec3 PerturbNormal(vec3 normal, vec3 dpdx, vec3 dpdy, float dhdx, float dhdy)
{
    return normalize(normal - CalculateSurfaceGradient(normal, dpdx, dpdy, dhdx, dhdy));
}

// Calculate the surface normal using screen-space partial derivatives of the height field
vec3 CalculateSurfaceNormal(vec3 position, vec3 normal, float height)
{
    vec3 dpdx = dFdx(position);
    vec3 dpdy = dFdy(position);
 
    float dhdx = dFdx(height);
    float dhdy = dFdy(height);
 
    return PerturbNormal(normal, dpdx, dpdy, dhdx, dhdy);
}

void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position);

void BlendMaterial(out Material blendedMat, Material lowSlopeMat, Material highSlopeMat, float blend)
{
	blendedMat.DiffuseColor = mix(lowSlopeMat.DiffuseColor, highSlopeMat.DiffuseColor, blend);
	blendedMat.SpecularColor = mix(lowSlopeMat.SpecularColor, highSlopeMat.SpecularColor, blend);
	blendedMat.SpecularPower = mix(lowSlopeMat.SpecularPower, highSlopeMat.SpecularPower, blend);
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

	Material mat;
	GetMaterial(mat, uvs, blendWeights, fs_in.Normal, fs_in.WorldPosition);

	normal = CalculateSurfaceNormal(fs_in.ViewPosition, fs_in.ViewNormal, mat.Height);
	//normal = CalculateSurfaceNormal(fs_in.WorldPosition, normal, mat.Height);
	//normal = dqTransformNormal(normal, u_ViewDQ);

	outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, TERRAIN_RENDERER_ID / 255)), packUnorm4x8(vec4(mat.SpecularColor, mat.SpecularPower / 255)), 0);
	//outPosition = vec4( fs_in.ViewPosition, uintBitsToFloat(packHalf2x16(fs_in.ViewNormal.xy)) );
	outPosition = vec4( fs_in.ViewPosition, uintBitsToFloat(packHalf2x16(normal.xy)) );
}

void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position)
{
	vec3 texCoord = uvs * u_TexScale;
	vec3 diffuseY = texture(u_textureSampler[0], vec3(texCoord.xz, 1)).xyz;
	vec3 diffuseX = texture(u_textureSampler[0], vec3(texCoord.zy, 1)).xyz;
	vec3 diffuseZ = texture(u_textureSampler[0], vec3(texCoord.xy, 1)).xyz;

	blendedMat.DiffuseColor = (blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ);

	//blendedMat.SpecularColor = vec3(0);
	vec3 specularY = texture(u_textureSampler[1], vec3(texCoord.xz, 1)).xyz;
	vec3 specularX = texture(u_textureSampler[1], vec3(texCoord.zy, 1)).xyz;
	vec3 specularZ = texture(u_textureSampler[1], vec3(texCoord.xy, 1)).xyz;

	blendedMat.SpecularColor = (blendWeights.y * specularY) + (blendWeights.x * specularX) + (blendWeights.z * specularZ);
	blendedMat.SpecularPower = 5;

	float heightY = texture(u_textureSampler[2], vec3(texCoord.xz, 1)).x;
	float heightX = texture(u_textureSampler[2], vec3(texCoord.zy, 1)).x;
	float heightZ = texture(u_textureSampler[2], vec3(texCoord.xy, 1)).x;

	blendedMat.Height = (blendWeights.y * heightY) + (blendWeights.x * heightX) + (blendWeights.z * heightZ);
}
