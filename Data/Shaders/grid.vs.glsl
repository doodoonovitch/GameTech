layout (location = 0) in vec3 vVertex;	//vertex position

uniform vec2 vGridSize;  

out VS_OUT
{
	vec4 Position;
	vec2 TexUV;
} vs_out;

void main()
{  
	vs_out.TexUV = (vVertex.xz / vGridSize) + vec2(0.5, 0.5);
	vs_out.Position = vec4(dqTransformPoint(u_ViewDQ, vVertex.xyz), 1);
	gl_Position = u_ProjMatrix * vs_out.Position;
}
