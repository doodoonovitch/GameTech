layout(location = 0) out vec4 vFragColor;

uniform vec4 u_VertexNormalColor;

void main(void)
{
	vFragColor = u_VertexNormalColor;
}
