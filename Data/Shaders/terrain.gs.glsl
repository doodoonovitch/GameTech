layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std430, binding = 0) buffer PerInstanceWorlMatrix
{
	vec4 data[];
} u_PerInstanceWorlMatrix;


in TES_OUT
{
	vec4 Blend;
	vec3 Normal;
	vec2 TexUV;
	flat int MapIndex;
} gs_in[3];

out GS_OUT
{
	vec4 Blend;
	vec3 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MapIndex;
} gs_out;

void main()
{  
	DualQuat modelDQ;
	int index = gs_in[0].MapIndex * 2;

	modelDQ.Qr = u_PerInstanceWorlMatrix.data[index];
	modelDQ.Qd = u_PerInstanceWorlMatrix.data[index + 1];
	
	for(int i = 0; i < gl_in.length(); ++i )
	{	
		gs_out.MapIndex = gs_in[i].MapIndex;
		gs_out.TexUV = gs_in[i].TexUV;
		gs_out.Blend = gs_in[i].Blend;

		vec3 pos = dqTransformPoint(modelDQ, gl_in[i].gl_Position.xyz);
		gs_out.Position = pos;

		gl_Position = u_ViewProjMatrix * vec4(pos, 1);

		gs_out.Normal = dqTransformNormal(gs_in[i].Normal, modelDQ);

		EmitVertex();
	}

	EndPrimitive();
}
