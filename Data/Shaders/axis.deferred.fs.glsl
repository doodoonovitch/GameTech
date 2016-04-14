layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

in VS_OUT
{
	vec4 Position;
	vec4 Color;
} fs_in;

void main(void)
{
	outPosition = vec4(fs_in.Position.xyz, uintBitsToFloat(packHalf2x16(vec2(0))));
	outData = uvec3(packUnorm4x8(vec4(fs_in.Color.xyz, AXIS_RENDERER_ID/255)), 0, 0);
}
