layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.1;
uniform float u_TriplanarBlendSharpness = 2.0;

uniform sampler2DArray u_HeightMap;
uniform sampler2DArray u_textureSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(int samplerIndex, vec3 p);
vec4 TexGetNormal(int samplerIndex, vec3 p);


in GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	//vec3 Normal;
	vec3 WorldNormal;
	vec3 ViewNormal;
	//flat int MapIndex;
} fs_in;

struct Material
{
	//vec3 Normal;
	vec3 DiffuseColor;
	vec3 SpecularColor;
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

/*
vec3 PerturbNormal ( vec3 surf_pos , vec3 surf_norm , float height )
{
	vec3 vSigmaS = dFdx ( surf_pos );
	vec3 vSigmaT = dFdx ( surf_pos );

	vec3 vN = surf_norm ; // normalized

	vec3 vR1 = cross ( vN, vSigmaT );
	vec3 vR2 = cross ( vSigmaS, vN );

	float fDet = dot ( vSigmaS , vR1 );

	float dBs = dFdx ( height );
	float dBt = dFdx ( height );

	vec3 vSurfGrad = sign ( fDet ) * ( dBs * vR1 + dBt * vR2 );
	return normalize ( abs ( fDet ) * vN - vSurfGrad );
}
*/
/*
mat3 CotangentFrame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}
*/

void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position);

void BlendMaterial(out Material blendedMat, Material lowSlopeMat, Material highSlopeMat, float blend)
{
	blendedMat.DiffuseColor = mix(lowSlopeMat.DiffuseColor, highSlopeMat.DiffuseColor, blend);
	blendedMat.SpecularColor = mix(lowSlopeMat.SpecularColor, highSlopeMat.SpecularColor, blend);
	blendedMat.Roughness = mix(lowSlopeMat.Roughness, highSlopeMat.Roughness, blend);
}

void main()
{
	vec3 normal = normalize(fs_in.WorldNormal);

	vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float d = length(fs_in.WorldPosition);
	float k = floor(log(d / 7.5) / log(2));
	float texScale = pow(0.5, k - 2);
	vec3 uvs = texScale * fs_in.WorldPosition;

	Material mat;
	GetMaterial(mat, uvs, blendWeights, normal, fs_in.WorldPosition);

	normal = normalize(fs_in.ViewNormal);
	normal = PerturbNormal(fs_in.ViewPosition, normal, mat.Height);

	//normal = PerturbNormal(fs_in.WorldPosition, normal, mat.Height);
	//normal = dqTransformNormal(normal, u_ViewDQ);

	//normal = fs_in.ViewNormal;

	WriteOutData(outAlbedoAndStatus, outSpecularAndRoughness, outEmissive, TERRAIN_RENDERER_ID , mat.DiffuseColor, mat.SpecularColor, mat.Roughness, vec3(0));

	//outData = uvec3(packUnorm4x8(vec4(mat.DiffuseColor, TERRAIN_RENDERER_ID / 255)), packUnorm4x8(vec4(mat.SpecularColor, mat.Roughness / 255)), 0);
	outPosition = fs_in.ViewPosition;
	outNormal = normal;
}
/*
void GetMaterial(out Material blendedMat, vec3 uvs, vec3 blendWeights, vec3 normal, vec3 position)
{
	const int textureLayer = 0;

	vec3 texCoord = uvs * u_TexScale;
	vec3 diffuseY = texture(u_textureSampler[0], vec3(texCoord.xz, textureLayer)).xyz;
	vec3 diffuseX = texture(u_textureSampler[0], vec3(texCoord.zy, textureLayer)).xyz;
	vec3 diffuseZ = texture(u_textureSampler[0], vec3(texCoord.xy, textureLayer)).xyz;

	blendedMat.DiffuseColor = (blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ);

	//blendedMat.SpecularColor = vec3(0);
	vec3 specularY = texture(u_textureSampler[1], vec3(texCoord.xz, textureLayer)).xyz;
	vec3 specularX = texture(u_textureSampler[1], vec3(texCoord.zy, textureLayer)).xyz;
	vec3 specularZ = texture(u_textureSampler[1], vec3(texCoord.xy, textureLayer)).xyz;

	blendedMat.SpecularColor = (blendWeights.y * specularY) + (blendWeights.x * specularX) + (blendWeights.z * specularZ);
	blendedMat.SpecularPower = 5;

	float heightY = texture(u_textureSampler[2], vec3(texCoord.xz, textureLayer)).x;
	float heightX = texture(u_textureSampler[2], vec3(texCoord.zy, textureLayer)).x;
	float heightZ = texture(u_textureSampler[2], vec3(texCoord.xy, textureLayer)).x;

	blendedMat.Height = (blendWeights.y * heightY) + (blendWeights.x * heightX) + (blendWeights.z * heightZ);

	//vec3 normalY = texture(u_textureSampler[2], vec3(texCoord.xz, 1)).xyz;
	//vec3 normalX = texture(u_textureSampler[2], vec3(texCoord.zy, 1)).xyz;
	//vec3 normalZ = texture(u_textureSampler[2], vec3(texCoord.xy, 1)).xyz;

	//blendedMat.Normal = (blendWeights.y * normalY) + (blendWeights.x * normalX) + (blendWeights.z * normalZ);	
}
*/
