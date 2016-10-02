//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;


uniform sampler2D u_HeightMapSampler;

uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform vec3 u_Scale;


in TCS_OUT
{
	vec2 TexUV;
	int MapIndex;
} tes_in[];

out TES_OUT
{
	vec2 TexUV;
	vec3 ViewPosition;
	vec3 Position;
	vec3 Normal;
	flat int MapIndex;
} tes_out;

const float u_dUV = 0.01;
const vec2 u_Step = vec2(1.0, 0.0);

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);
	
#ifdef PRECOMPUTE_NORMAL
	vec3 temp = texture(u_HeightMapSampler, tc).rgb;
	p.y = temp.r;
	vec3 normal = vec3(-temp.g, 1, -temp.b);
#else
	p.y = texture(u_HeightMapSampler, tc).r;

	float dUV = u_dUV;
	float h0 = texture(u_HeightMapSampler, vec2(tc.x - dUV, tc.y)).r;
	float h1 = texture(u_HeightMapSampler, vec2(tc.x + dUV, tc.y)).r;
	float h2 = texture(u_HeightMapSampler, vec2(tc.x, tc.y - dUV)).r;
	float h3 = texture(u_HeightMapSampler, vec2(tc.x, tc.y + dUV)).r;

	vec3 normal = vec3(h0 - h1, 2 /** u_MapSize.x * u_dUV*/, h2 - h3);
#endif
	vec4 viewPos = vec4(dqTransformPoint(u_ViewDQ, p.xyz), 1);

	tes_out.Position = p.xyz;
	tes_out.ViewPosition = viewPos.xyz;
	tes_out.TexUV = tc;
	tes_out.MapIndex = tes_in[0].MapIndex;
	tes_out.Normal = normal;
	
	gl_Position = u_ProjMatrix * viewPos;
}


