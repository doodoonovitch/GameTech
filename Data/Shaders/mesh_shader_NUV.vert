layout (location = POSITION_ATTRIBUTE) in vec3 vPosition;
layout (location = NORMAL_ATTRIBUTE) in vec3 vNormal;
layout (location = UV_ATTRIBUTE) in vec2 vUV;
layout (location = MODEL_MATRIX_ATTRIBUTE) in mat4 mPerInstanceModel;


smooth out vec2 vUVout;
smooth out vec3 vEyeSpacePosition;

void main()
{
	vUVout=vUV; 
	
	mat4 mMV = mView * mPerInstanceModel;

	vEyeSpacePosition = (mMV * vec4(vPosition,1)).xyz; 

	gl_Position = mProj * vec4(vEyeSpacePosition,1);
}