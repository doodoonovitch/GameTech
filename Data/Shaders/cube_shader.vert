#version 330
  
layout(location = 0) in vec3 vVertex;	//vertex position
layout(location = 1) in vec2 vUV;	//vertex color

uniform mat4 MVP;  

smooth out vec2 vUVOut;				//texture color (output)

void main()
{  
	vUVOut = vUV;

	gl_Position = MVP*vec4(vVertex.xyz,1);
}
