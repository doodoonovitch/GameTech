layout(location = 0) out vec4 outColor;

in GS_OUT
{
	vec4 Blend;
	vec3 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MapIndex;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

