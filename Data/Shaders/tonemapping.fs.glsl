layout(location = 0) out vec4 vFragColor;

in VS_OUT
{
	vec2 TexUV;
} fs_in;

uniform float u_Exposure = 0.2;
uniform float u_InvGamma = 1.0 / 1.0;
uniform sampler2D u_HdrBuffer;

void main(void)
{
	vec3 hdrColor = texture(u_HdrBuffer, fs_in.TexUV).rgb;

	vec3 mapped = vec3(1.0) - exp(-hdrColor * u_Exposure);
	mapped = pow(mapped, vec3(u_InvGamma));

	vFragColor =  vec4(mapped, 1.0);
}
