#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=256) out; 
 
//uniforms
uniform int subDivisions;		 //the number of subdivisions
uniform mat4 mMVP;				 //combined view porjection matrix

void main()
{
	//get the object space vertex positions
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = gl_in[1].gl_Position;
	vec4 v2 = gl_in[2].gl_Position; 

	//determine the size of each sub-division 
	float dx = abs(v0.x-v2.x) / subDivisions;
	float dz = abs(v0.z-v1.z) / subDivisions;

	float x = v0.x;
	float z = v0.z;

	//loop through all sub-divisions and emit vertices
	//after mutiplying the object space vertex positions
	//with the combined modelview projection matrix. We 
	//move first in x axis, once we reach the edge, we 
	//reset x to the initial x value while incrementing 
	//the z value.
	for(int j = 0; j < (subDivisions * subDivisions); j++) 
	{
		gl_Position =  mMVP * vec4(x,0,z,1);        EmitVertex();		
		gl_Position =  mMVP * vec4(x,0,z+dz,1);     EmitVertex();				  
		gl_Position =  mMVP * vec4(x+dx,0,z,1);     EmitVertex(); 
		gl_Position =  mMVP * vec4(x+dx,0,z+dz,1);  EmitVertex();
		EndPrimitive();	 
		x+=dx;

		if((j+1) % subDivisions == 0) 
		{
		   x = v0.x;
		   z += dz;
		}	
	}	
}