layout (quads, fractional_odd_spacing) in;

uniform int u_PatchPerTexture;
uniform vec3 u_Scale;

uniform sampler2DArray u_HeightMap;

in TCS_OUT
{
	vec2 TexUV;
	int InstanceId;
} tes_in;

out TES_OUT
{
	vec3 Position;
	vec2 TexUV;
} tes_out;

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	int layer = tes_in[0].InstanceId / u_PatchPerTexture;
	p.y += texture(u_HeightMap, vec3(tc, layer)).r * u_Scale.z;

	tes_out.Position = vec4(dqTransformPoint(u_ViewDQ, p.xyz), 1.0);
	gl_Position = u_ProjMatrix * vec4(tes_out.Position, 1.0);
	tes_out.TexUV = tc;
}


