layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

