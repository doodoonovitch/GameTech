layout(location = 0) in vec3 vVertex;	//vertex position
layout(location = 1) in vec4 vColor;	//vertex color

out VS_OUT
{
	vec4 Color;
} vs_out;

void main()
{  
	vs_out.Color = vColor;
	gl_Position = u_ViewProjMatrix * vec4(vVertex.xyz, 1);
}
