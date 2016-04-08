layout (quads, fractional_odd_spacing) in;

uniform vec3 u_Scale;

uniform sampler2DArray u_HeightMap;

in TCS_OUT
{
	vec2 TexUV;
	int Layer;
	int LocalIndex;
} tes_in[];

out TES_OUT
{
	vec3 Position;
	vec2 TexUV;
	flat int LocalIndex;
} tes_out;

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	p.y = texture(u_HeightMap, vec3(tc, tes_in[0].Layer)).r * u_Scale.y;

	tes_out.Position = dqTransformPoint(u_ViewDQ, p.xyz);
	gl_Position = u_ProjMatrix * vec4(tes_out.Position, 1.0);
	tes_out.TexUV = tc;
	tes_out.LocalIndex = tes_in[0].LocalIndex;
}


