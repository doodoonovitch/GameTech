layout(location = 0) out vec3 outPosition;
layout(location = 1) out uvec4 outRGBA32UI;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;

void main(void)
{
	outPosition = fs_in.Position.xyz;

	uvec4 data = uvec4(0);
	data.x = packHalf2x16(fs_in.TexUV);
	data.y = packHalf2x16(vec2(0, fs_in.Normal.z));
	data.z = packHalf2x16(fs_in.Normal.xy);
	data.w = fs_in.MaterialIndex;

	outRGBA32UI = data;
}
