#version 430

layout (binding = 0, std140, shared) uniform FrameData
{
	mat4 mView;
	mat4 mProj;
};
