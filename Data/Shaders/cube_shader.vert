
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexUV;
layout (location = 2) in mat4 mPerInstanceModel;

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