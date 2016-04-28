layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform sampler2D u_gBufferPosition;
uniform sampler2D u_gBufferNormal;

//in VS_OUT
//{
//	vec2 TexUV;
//} gs_in[3];

//out GS_OUT
//{
//	vec4 Position;
//} gs_out;

struct FragmentInfo
{
    vec3 Position;
    vec3 Normal;
};

void UnpackFromGBuffer(out FragmentInfo fi, vec2 texUV)
{
    vec4 encPositionNormal = texture(u_gBufferPosition, texUV, 0);
	fi.Position = encPositionNormal.xyz;
	fi.Normal = texture(u_gBufferNormal, texUV, 0).xyz;
}

void main()
{  
	FragmentInfo fi;
	for(int i = 0; i < gl_in.length(); ++i )
	{
		vec2 texUV = gl_in[i].gl_Position.xy;
		UnpackFromGBuffer(fi, texUV);

		vec4 projPos = u_ProjMatrix * vec4(fi.Position, 1);
		gl_Position = projPos;
		EmitVertex();

		vec4 position = vec4(fi.Position + (fi.Normal * u_NormalMagnitude), 1);
		gl_Position = u_ProjMatrix * position;
		EmitVertex();

		EndPrimitive();
	}
}
