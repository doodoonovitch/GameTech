layout(location = 0) in vec3 in_Position;	//vertex position



out VS_OUT
{
	vec3 TexCoords;
} vs_out;

void main()
{  
	//vec4 pos = u_ProjMatrix * vec4(dqTransformNormal(in_Position, u_ViewDQ), 1);
	vec4 pos = u_ProjMatrix * u_ViewMatrix * vec4(in_Position, 0);
	gl_Position = pos.xyww;
	vs_out.TexCoords = in_Position;
}
