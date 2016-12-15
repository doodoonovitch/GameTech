layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

uniform int u_Frame0;
uniform vec2 u_Scale;
uniform sampler2DArray u_HeightMapsSampler;

out VS_OUT
{
	vec2 TexUV;
} vs_out;



void main()
{  
	vec4 pos = vec4(in_Position.x + u_ViewPosition.x, in_Position.y, in_Position.z + u_ViewPosition.z, 1);
	//vec2 orig = floor(u_ViewPosition.xz);
	//vec4 pos = vec4(in_Position.x + orig.x, in_Position.y, in_Position.z + orig.y, 1);

	vec2 texUV = pos.xz * u_Scale.x;

	float h = texture(u_HeightMapsSampler, vec3(texUV, u_Frame0)).r;

	h = h * 0.5 - 0.5;
	pos.y += h * u_Scale.y;

	vs_out.TexUV = texUV;
	gl_Position = u_ViewProjMatrix * pos;
}