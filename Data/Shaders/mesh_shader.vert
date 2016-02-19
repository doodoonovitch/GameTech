#version 330 core
 
layout(location = 0) in vec3 vVertex;	 // vertex position

//uniforms for projection, modelview and normal matrices
uniform(location = 1) mat4 mP; 
uniform(location = 2) mat4 mMV;

//shader outputs to the fragment shader
smooth out vec4 vColor;					// Color
smooth out vec3 vEyeSpaceNormal;    	// eye space normals
smooth out vec3 vEyeSpacePosition;		// eye space positions

void main()
{
   //output the texture coordinates
	vColor=vec4(1, 1, 1, 1); 
	
	//multiply the object space vertex position with the modelview matrix 
	//to get the eye space position  
	vEyeSpacePosition = (mMV * vec4(vVertex, 1)).xyz; 

	//multiply the projection matrix with the eye space position to get
	//the clipspace postion
	gl_Position = mP * vec4(vEyeSpacePosition, 1);
}