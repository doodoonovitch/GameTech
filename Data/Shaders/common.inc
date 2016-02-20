#version 330

layout (std140, shared) uniform FrameData
{
	mat4 mView;
	mat4 mProj;
};
