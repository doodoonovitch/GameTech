layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec3 TexCoords;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

