layout(location = 0) out vec4 outColor;

in TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	//vec3 Normal;
	//vec3 Tangent;
	flat int MapIndex;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

