layout(location = 0) in vec3 vVertex;	//vertex position
layout(location = 1) in vec4 vColor;	//vertex color

smooth out vec4 vColorOut;				//texture color (output)

void main()
{  
	vColorOut = vColor;

	gl_Position = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, vVertex.xyz), 1);
}