layout(location = 0) out vec4 outColor;

in GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	//vec3 Normal;
	vec3 WorldNormal;
	vec3 ViewNormal;
	//flat int MapIndex;
} fs_in;


// ---------------------------------------------------------------------------


void main(void)
{
	outColor = u_WireFrameDrawColor;
}

