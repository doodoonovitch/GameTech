#version 330
  
layout (location = 0) in vec3 vVertex;	//vertex position

uniform mat4 mMVP;  
uniform vec2 vGridSize;  

smooth out vec2 vUVOut;				//texture color (output)

void main()
{  
	vUVOut = (vVertex.xz / vGridSize) + vec2(0.5, 0.5);

	gl_Position = mMVP * vec4(vVertex.xyz,1);
}
