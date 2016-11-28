layout(location = 0) out vec4 outColor;

in VS_OUT
{
	vec4 Color;
	vec2 TexUV;
	//flat uint LayerIndexAndStatus;
	flat uint LayerIndex;
} fs_in;

void main(void)
{
	outColor = u_WireFrameDrawColor;
}
