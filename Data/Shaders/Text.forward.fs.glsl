layout(location = 0) out vec4 outColor;

uniform sampler2DArray u_FontTextureSampler;

in VS_OUT
{
	vec4 Color;
	vec2 TexUV;
	//flat uint LayerIndexAndStatus;
	flat uint LayerIndex;
} fs_in;

void main(void)
{
	outColor = fs_in.Color * texture(u_FontTextureSampler, vec3(fs_in.TexUV, fs_in.LayerIndex)).r;
}
