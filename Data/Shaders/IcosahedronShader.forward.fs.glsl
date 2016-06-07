layout(location = 0) out vec4 outColor;

uniform vec4 u_DrawColor;

in GS_OUT
{
	vec3 Normal;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_DrawColor;
}

