
layout (location = POSITION_ATTRIBUTE) in vec3 vPos;
layout (location = UV_ATTRIBUTE) in vec2 vTexUV;
layout (location = MODEL_MATRIX_ATTRIBUTE) in mat4 mPerInstanceModel;

out VS_OUT
{
	vec2 vTexUV;
	mat4 mModel;
} vs_out;

void main()
{  
	vs_out.vTexUV = vTexUV;
	vs_out.mModel = mPerInstanceModel;
	gl_Position = vec4(vPos.xyz, 1);
}


/*
void main()
{
	vs_out.vTexUV = vTexUV;
	gl_Position = mProj * mView * mPerInstanceModel * vec4(vPos.xyz, 1);
}
*/