layout (location = 0) in vec3 vVertex;	//vertex position

uniform vec2 vGridSize;  

out VS_OUT
{
	vec2 TexUV;
} vs_out;

void main()
{  
	vs_out.TexUV = (vVertex.xz / vGridSize) + vec2(0.5, 0.5);
	gl_Position = u_ViewProjMatrix * vec4(vVertex.xyz, 1);
}
