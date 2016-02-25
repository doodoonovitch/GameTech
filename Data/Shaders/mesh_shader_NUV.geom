layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform samplerBuffer texBufferSampler;

in VS_OUT
{
	vec3 vNormal;
	vec2 vTexUV;
	int iInstanceId;
} gs_in[3];

out vec3 vNormal;
out vec2 vTexUV;

void main()
{  
	int index = gs_in[0].iInstanceId * 4;
	vec4 col1 = texelFetch(texBufferSampler, index);
	vec4 col2 = texelFetch(texBufferSampler, index + 1);
	vec4 col3 = texelFetch(texBufferSampler, index + 2);
	vec4 col4 = texelFetch(texBufferSampler, index + 3);

	mat4 mModel = mat4(col1, col2, col3, col4);

	mat4 mMV = u_ViewMatrix * mModel;
	mat4 mMVP = u_ProjMatrix * mMV;
	mat3 m3MV = mat3(mMV);

	gl_Position = mMVP * gl_in[0].gl_Position;
	vNormal = m3MV * gs_in[0].vNormal;
	vTexUV = gs_in[0].vTexUV;
	EmitVertex();

	gl_Position = mMVP * gl_in[1].gl_Position;
	vNormal = m3MV * gs_in[1].vNormal;
	vTexUV = gs_in[1].vTexUV;
	EmitVertex();

	gl_Position = mMVP * gl_in[2].gl_Position;
	vNormal = m3MV * gs_in[2].vNormal;
	vTexUV = gs_in[2].vTexUV;
	EmitVertex();

	EndPrimitive();
}
