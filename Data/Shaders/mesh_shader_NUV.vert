layout (location = POSITION_ATTRIBUTE) in vec3 vPosition;
layout (location = NORMAL_ATTRIBUTE) in vec3 vNormal;
layout (location = UV_ATTRIBUTE) in vec2 vUV;


out VS_OUT
{
	vec3 vNormal;
	vec2 vTexUV;
	int iInstanceId;
} vs_out;

void main()
{
	vs_out.vNormal = vNormal;
	vs_out.vTexUV = vUV; 
	
	gl_Position = vec4(vPosition.xyz, 1);
}