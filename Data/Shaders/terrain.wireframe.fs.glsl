layout(location = 0) out vec4 outColor;

in TES_OUT
{
	vec4 Blend;
	vec3 Position;
	vec2 TexUV;
	//float MipmapLevel;
	flat int MapIndex;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

