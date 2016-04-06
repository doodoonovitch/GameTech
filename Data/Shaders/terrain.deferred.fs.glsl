layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

in TES_OUT
{
	vec3 Position;
	vec2 TexUV;
} fs_in;

void main()
{
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(CombineRenderIdAndMaterialIndex(CUBE_RENDERER_ID, 0)));

	vec3 normal = vec3(0);
	outData = uvec2(packHalf2x16(fs_in.TexUV.xy), packHalf2x16(normal.xy));
}


