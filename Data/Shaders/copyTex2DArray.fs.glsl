layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;

uniform sampler2DArray u_TextureSampler;
uniform int u_LayerIndex;

void main(void)
{
	vec4 color = texture(u_TextureSampler, vec3(fs_in.TexUV, u_LayerIndex));

	vFragColor =  vec4(color.xyz, 1);
}