layout (location = POSITION_ATTRIBUTE) in vec4 in_Position;


out VS_OUT
{
	vec3 Center;
	float Radius;
} vs_out;

void main()
{  
	vs_out.Center = in_Position.xyz;
	vs_out.Radius = in_Position.w;

	gl_Position = vec4(0., 0., 0., 1.);
}

