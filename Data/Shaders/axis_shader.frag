#version 330

layout(location=0) out vec4 vFragColor;

smooth in vec4 vColorOut;			//color from the vertex shader


void main(void)
{
	vFragColor = vColorOut;
}
