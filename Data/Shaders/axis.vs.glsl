layout(location = 0) in vec3 vVertex;	//vertex position
layout(location = 1) in vec4 vColor;	//vertex color

out VS_OUT
{
	vec4 Position;
	vec4 Color;
} vs_out;

void main()
{  
	vs_out.Color = vColor;
	vs_out.Position = vec4(dqTransformPoint(u_ViewDQ, vVertex.xyz), 1);

	gl_Position = u_ProjMatrix * vs_out.Position;
}
