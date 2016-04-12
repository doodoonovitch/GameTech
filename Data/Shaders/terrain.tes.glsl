//layout (quads, fractional_odd_spacing) in;
layout (quads, fractional_even_spacing) in;
//layout (quads, equal_spacing) in;

uniform vec3 u_Scale;

uniform sampler2DArray u_HeightMap;

in TCS_OUT
{
	vec2 TexUV;
	int Layer;
} tes_in[];

out TES_OUT
{
	vec2 TexUV;
	vec3 Normal;
	flat int Layer;
} tes_out;

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	vec3 texCoord = vec3(tc, tes_in[0].Layer);
	p.y = texture(u_HeightMap, texCoord).r * u_Scale.y;

	float nX = textureOffset(u_HeightMap, texCoord, ivec2(-1, 0)).r - textureOffset(u_HeightMap, texCoord, ivec2(1, 0)).r;
	float nZ = textureOffset(u_HeightMap, texCoord, ivec2(0, -1)).r - textureOffset(u_HeightMap, texCoord, ivec2(0, 1)).r;
	vec3 normal = normalize(vec3(nX * u_Scale.y, (u_Scale.x + u_Scale.z), nZ * u_Scale.y));

	gl_Position = vec4(p.xyz, 1);
	tes_out.TexUV = tc;
	tes_out.Layer = tes_in[0].Layer;
	tes_out.Normal = normal;
}


