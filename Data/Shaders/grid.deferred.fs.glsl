layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

in VS_OUT
{
	vec4 Position;
	vec2 TexUV;
} fs_in;


void main(void)
{
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(GRID_RENDERER_ID, 0)));
	outData = uvec2(packHalf2x16(fs_in.TexUV.xy), 0);
}
