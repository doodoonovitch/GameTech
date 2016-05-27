layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec4 Position;
	vec4 Color;
} fs_in;

void main(void)
{
	outColor = fs_in.Color;
}
