layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;

void main(void)
{
	outPosition = vec4(fs_in.Position.xyz, intBitsToFloat(fs_in.MaterialIndex));

	outData = uvec2(packHalf2x16(fs_in.TexUV.xy), packHalf2x16(fs_in.Normal.xy));
}
