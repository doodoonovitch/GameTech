layout(location = 0) out vec4 outColor;

uniform sampler2D u_CompassTextureSampler;

in VS_OUT
{
	vec2 TexUV;
} fs_in;

void main(void)
{
	vec4 color = texture(u_CompassTextureSampler, fs_in.TexUV);
	outColor = vec4(color.xyz, length(color.xyz));
	//outColor = vec4(1, 0, 0, 1);
}
