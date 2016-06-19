layout(location = 0) out vec4 outColor;

in GS_OUT
{
	//vec3 Normal;
	vec4 Color;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = fs_in.Color;
}

