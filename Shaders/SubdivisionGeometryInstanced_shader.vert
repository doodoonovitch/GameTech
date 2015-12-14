#version 330 core
  
layout(location=0) in vec3 vVertex;  //object space vertex position

uniform mat4 M[4];		//modeling matrix for each instance
uniform vec4 color[4];	//color for each instance

out VS_OUT {
    vec4 color;
} vs_out;

void main()
{    
	//get the world space position of each instance vertex
	gl_Position =  M[gl_InstanceID]*vec4(vVertex, 1);
	vs_out.color = color[gl_InstanceID];
}
