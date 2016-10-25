layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform sampler2D u_gDepthMap;
uniform sampler2D u_gBufferNormal;

in VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} gs_in[3];

out GS_OUT
{
	vec4 Color;
} gs_out;

struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
};

void UnpackFromGBuffer(out FragmentInfo fi, vec2 texUV, vec2 viewRay)
{
    fi.Position = PositionFromDepth(texture(u_gDepthMap, texUV, 0).x, viewRay);
	fi.Normal = texture(u_gBufferNormal, texUV, 0).xyz;
}

void main()
{  
	FragmentInfo fi;
	for(int i = 0; i < gl_in.length(); ++i )
	{
		UnpackFromGBuffer(fi, gs_in[i].TexUV, gs_in[i].ViewRay);

		vec4 projPos = u_ProjMatrix * vec4(fi.Position, 1);
		gl_Position = projPos;
		gs_out.Color = vec4(u_VertexNormalColor.xyz, 1);
		//gs_out.Color = vec4(fi.Normal, 1);
		EmitVertex();

		vec4 position = vec4(fi.Position + (fi.Normal * u_NormalMagnitude), 1);
		gl_Position = u_ProjMatrix * position;
		gs_out.Color = vec4(u_VertexNormalColor.xzy, 1);
		//gs_out.Color = vec4(fi.Normal, 1);
		EmitVertex();

		EndPrimitive();
	}
}
