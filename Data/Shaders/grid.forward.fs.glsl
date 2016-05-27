layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec4 Position;
	vec2 TexUV;
} fs_in;


void main(void)
{
	outColor = vec4(fs_in.TexUV.xy, 0, 1);
}