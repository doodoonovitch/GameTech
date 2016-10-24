layout(location = 0) out vec4 outColor;

in GS_OUT
{
	vec3 Normal;
	vec3 Tangent;
	vec2 TexUV;
	flat int MaterialIndex;
	flat DualQuat ViewModelDQ;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

