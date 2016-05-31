layout(location = 0) out vec4 outColor;

uniform vec4 u_WireFrameDrawColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

