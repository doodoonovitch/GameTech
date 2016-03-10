layout(location = 0) out uvec4 outColor0;
layout(location = 1) out vec4 outColor1;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;


void main(void)
{
	vec4 data1 = vec4(Position.xyz, fs_in.Normal.z);
	uvec4 data0 = uvec4(0);

	data0.x = packHalf2x16(fs_in.TexUV);
	data0.y = packHalf2x16(fs_in.Normal.xy);
	data0.w = fs_in.MaterialIndex;

	outColor0 = data0;
	outColor1 = data1;
}
