layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

uniform samplerBuffer u_materialDataSampler;
uniform sampler2DArray u_normalMapSampler[MAX_TEXTURE_SAMPLER];

vec4 TexGet(int samplerIndex, vec3 p);


in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ViewModelDQ;
} fs_in;

void main(void)
{
	vec4 matData = texelFetch(u_materialDataSampler, fs_in.MaterialIndex + 2);
	uint bitfieldValue = floatBitsToUint(matData.w);
	int normalTextureIndex = int(bitfieldValue & uint(255));
	int normalSamplerIndex = int((bitfieldValue >> 8) & uint(255));

	vec3 bumpMapNormal = TexGet(normalSamplerIndex, vec3(fs_in.TexUV, normalTextureIndex)).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);

	vec3 normal = ComputeBumpedNormal(fs_in.Normal, fs_in.Tangent, bumpMapNormal);
	normal = dqTransformNormal(normal, fs_in.ViewModelDQ);
	
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(CUBE_RENDERER_ID, fs_in.MaterialIndex)));
	//outPosition = vec4(gl_FragCoord.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(CUBE_RENDERER_ID, fs_in.MaterialIndex)));

	outData = uvec2(packHalf2x16(fs_in.TexUV.xy), packHalf2x16(normal.xy));
}
