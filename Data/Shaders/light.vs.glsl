
layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;
layout (location = UV_ATTRIBUTE) in vec2 in_TexUV;

out VS_OUT
{
	vec2 TexUV;
} vs_out;

void main()
{  
	vs_out.TexUV = in_TexUV;
	gl_Position = vec4(in_Position, 1);
}

