layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec3 Color;
} fs_in;

void main(void)
{
	outColor = vec4(fs_in.Color, 1);
}
