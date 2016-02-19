
layout (location = 0) in vec3 vVertex;			// vertex position
layout (location = 1) in vec2 vUV;				// vertex color
layout (location = 2) in mat4 mModel;			// per instance Model matrix

smooth out vec2 vUVOut;							//texture color (output)

void main()
{  
	vUVOut = vUV;

	mat4 mMVP = mProj * mView * mModel;

	gl_Position = mMVP * vec4(vVertex.xyz, 1);
}
