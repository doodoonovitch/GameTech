layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec2 vTexUV;
	mat4 mModel;
} gs_in[3];

out vec2 vTexUV;

void main()
{  
	mat4 mMVP = mProj * mView * gs_in[0].mModel;

	gl_Position = mMVP * gl_in[0].gl_Position;
	vTexUV = gs_in[0].vTexUV;
	EmitVertex();

	gl_Position = mMVP * gl_in[1].gl_Position;
	vTexUV = gs_in[1].vTexUV;
	EmitVertex();

	gl_Position = mMVP * gl_in[2].gl_Position;
	vTexUV = gs_in[2].vTexUV;
	EmitVertex();

	EndPrimitive();
}
