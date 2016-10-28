layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

out VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} vs_out;

void main()
{  
	vs_out.TexUV = (in_Position.xy + vec2(1.0)) * 0.5;
	
	vs_out.ViewRay = in_Position.xy / vec2(u_ProjMatrix[0][0], u_ProjMatrix[1][1]);

	gl_Position = vec4(in_Position, 1);
}

