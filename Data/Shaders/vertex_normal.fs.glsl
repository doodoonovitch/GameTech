layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

in GS_OUT
{
	vec4 Position;
	vec4 Color;
} fs_in;

void main(void)
{
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(VERTEX_NORMAL_RENDERER_ID, 0)));
	outData = uvec2(packHalf2x16(fs_in.Color.xy), packHalf2x16(fs_in.Color.zw));
}
