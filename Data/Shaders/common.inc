#version 420

#define	POSITION_ATTRIBUTE			0
#define	UV_ATTRIBUTE				1
#define	NORMAL_ATTRIBUTE			2
#define	TANGENT_ATTRIBUTE			3
#define	BINORMAL_ATTRIBUTE			4
#define	MATERIALID_ATTRIBUTE		5
#define MODEL_MATRIX_ATTRIBUTE		6


layout (std140, shared) uniform FrameData
{
	mat4 mView;
	mat4 mProj;
};
