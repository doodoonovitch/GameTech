layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

out VS_OUT
{
	vec2 TexUV;
} vs_out;

void main()
{  
	vs_out.TexUV = (in_Position.xy + vec2(1.0)) * 0.5;
	gl_Position = vec4(in_Position, 1);
}

