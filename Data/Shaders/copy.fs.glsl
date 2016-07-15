layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;

uniform sampler2D u_SourceBuffer;

void main(void)
{
	vec4 color = texture(u_SourceBuffer, fs_in.TexUV);

	vFragColor =  color;
}