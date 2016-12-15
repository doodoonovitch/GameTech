layout (location = 0) in vec3 in_Position;	//vertex position

uniform vec2 vGridSize;  

out VS_OUT
{
	vec2 TexUV;
} vs_out;

void main()
{  
	float h = 0;
	//vec2 orig = floor(u_ViewPosition.xz);
	vec2 orig = u_ViewPosition.xz;
	vec4 pos = vec4(in_Position.x + orig.x, in_Position.y + h, in_Position.z + orig.y, 1);

	vs_out.TexUV = pos.xz;
	gl_Position = u_ViewProjMatrix * pos;
}
