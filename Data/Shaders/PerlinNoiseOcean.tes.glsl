//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;

uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform ivec2 u_HeightMapTextureSize;
uniform sampler2D u_HeightMapSampler;


in TCS_OUT
{
	vec2 TexUV;
	int MapIndex;
} tes_in[];

#ifdef PER_VERTEX_NORMAL

uniform float u_MaxAmplitude;
uniform sampler2D u_NormalMapSampler;

out TES_OUT
{
	vec2 TexUV;
	vec3 Position;
	vec3 Normal;
	flat int MapIndex;
} tes_out;

#else

out TES_OUT
{
	vec2 TexUV;
	vec3 Position;
	flat int MapIndex;
} tes_out;

#endif


void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	p.y = texture(u_HeightMapSampler, tc).r;
	//ivec2 itc = ivec2(tc * u_HeightMapTextureSize);
	//p.y = texelFetch(u_HeightMapSampler, itc, 0).r;

#ifdef PER_VERTEX_NORMAL
	float nX = textureOffset(u_HeightMapSampler, tc, ivec2(-1, 0)).r - textureOffset(u_HeightMapSampler, tc, ivec2(1, 0)).r;
	float nZ = textureOffset(u_HeightMapSampler, tc, ivec2(0, -1)).r - textureOffset(u_HeightMapSampler, tc, ivec2(0, 1)).r;
	tes_out.Normal = normalize(vec3(nX, 2, nZ));
	//tes_out.Normal = normalize(texture(u_NormalMapSampler, tc).rgb);
#endif

	tes_out.Position = p.xyz;
	tes_out.TexUV = tc;
	tes_out.MapIndex = tes_in[0].MapIndex;
	
	gl_Position = u_ViewProjMatrix * p;
}


