layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

in TES_OUT
{
	vec3 Position;
	vec2 TexUV;
	flat int LocalIndex;
} fs_in;

void main()
{
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(TERRAIN_RENDERER_ID, 0)));

	//vec3 normal = vec3(fs_in.LocalIndex, 0, 0);
	outData = uvec2(packHalf2x16(fs_in.TexUV.xy), uint(fs_in.LocalIndex));
}


