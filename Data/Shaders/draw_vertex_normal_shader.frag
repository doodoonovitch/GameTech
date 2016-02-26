layout(location = 0) out vec4 vFragColor;

in GS_OUT
{
	vec4 Color;
} fs_in;

void main(void)
{
	vFragColor = fs_in.Color;
}
