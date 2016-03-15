layout (location = 0) in vec3 vVertex;	//vertex position

uniform vec2 vGridSize;  

smooth out vec2 vUVOut;				//texture color (output)

void main()
{  
	vUVOut = (vVertex.xz / vGridSize) + vec2(0.5, 0.5);

	gl_Position = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, vVertex.xyz), 1);
}