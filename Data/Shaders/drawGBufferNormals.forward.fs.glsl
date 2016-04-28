layout(location = 0) out vec4 vFragColor;

void main(void)
{
	vFragColor =  u_VertexNormalColor.xzyw;
}
