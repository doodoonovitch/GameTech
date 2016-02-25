layout(location = 0) out vec4 vFragColor;

in vec3 vNormal;
in vec2 vTexUV;

uniform sampler2D texSampler1;

void main(void)
{
	vFragColor = texture(texSampler1, vTexUV);
}
