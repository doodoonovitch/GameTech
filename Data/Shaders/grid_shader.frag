#version 330

layout(location=0) out vec4 vFragColor;

smooth in vec2 vUVOut;			//color from the vertex shader


void main(void)
{
	vFragColor = vec4(vUVOut, 1, 1);
}
