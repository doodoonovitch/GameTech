#version 330 core
 
layout(location=0) out vec4 vFragColor;	//fragment shader output

//input form the vertex shader
in vec4 vertex_color;		//interpolated colour to fragment shader

void main()
{
   vFragColor = vertex_color;
}
