layout(location = 0) in vec3 vVertex;	//vertex position
layout(location = 1) in vec4 vColor;	//vertex color

uniform vec2 vGridSize;  

smooth out vec4 vColorOut;				//texture color (output)

void main()
{  
	vColorOut = vColor;

	gl_Position = mProj * mView * vec4(vVertex.xyz,1);
}
