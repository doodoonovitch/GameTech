//layout (quads, fractional_odd_spacing) in;
layout (quads, fractional_even_spacing) in;
//layout (quads, equal_spacing) in;

const int c_MaxWavesToSum = 4;

uniform vec3 u_Direction[c_MaxWavesToSum];

uniform float[c_MaxWavesToSum] u_WaveLength = float[c_MaxWavesToSum](1.000f, 0.620f, 0.280f, 0.500f);
uniform float[c_MaxWavesToSum] u_Amplitude = float[c_MaxWavesToSum](0.08f, 0.07f, 0.06f, 0.08f);
uniform float[c_MaxWavesToSum] u_Velocity = float[c_MaxWavesToSum](0.05f, 0.06f, 0.04f, 0.05f);
uniform float[c_MaxWavesToSum] u_Steepness = float[c_MaxWavesToSum](10.0f, 2.0f, 1.0f, 3.0f);




in TCS_OUT
{
	//vec2 TexUV;
	int MapIndex;
} tes_in[];

out TES_OUT
{
	//vec2 TexUV;
	vec3 Normal;
	flat int MapIndex;
} tes_out;

void main()
{
	//vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	//vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	//vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

	gl_Position = vec4(p.xyz, 1);
	//tes_out.TexUV = tc;
	tes_out.MapIndex = tes_in[0].MapIndex;
	tes_out.Normal = normal;
}


