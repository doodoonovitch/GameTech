#version 330

layout(location=0) out vec4 vFragColor;

smooth in vec2 vUVOut;			//color from the vertex shader

uniform sampler2D texSampler1;

void main(void)
{
	vFragColor = texture(texSampler1, vUVOut);
}
