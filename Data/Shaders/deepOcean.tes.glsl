//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;

const int c_MaxWavesToSum = 1;

uniform vec3[c_MaxWavesToSum] u_Direction;
uniform float[c_MaxWavesToSum] u_WaveLength;
uniform float[c_MaxWavesToSum] u_Frequency;
uniform float[c_MaxWavesToSum] u_Amplitude;
uniform float[c_MaxWavesToSum] u_Velocity;
uniform float[c_MaxWavesToSum] u_Steepness;
uniform float[c_MaxWavesToSum] u_Phase;




in TCS_OUT
{
	//vec2 TexUV;
	int MapIndex;
} tes_in[];

out TES_OUT
{
	vec3 Normal;
	//vec3 Tangent;
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

	float H = 0;
	vec3 normal = vec3(0);
	float t = float(u_TimeDeltaTime.x);

	for(int i = 0; i < c_MaxWavesToSum; ++i)
	{
		float dirPos = dot(u_Direction[i].xz, p.xz);
		float S = dirPos * u_Frequency[i] + t * u_Phase[i];
		float halfOfSinSplusOne = 0.5 * (1 + sin(S));
		H += u_Amplitude[i] * pow(halfOfSinSplusOne, u_Steepness[i]);

		float cosS = cos(S);
		float dhCommon = u_Steepness[i] * u_Frequency[i] * u_Amplitude[i] * cosS;

		if (u_Steepness[i] > 1)
		{
			float halfOfSinSplusOnePowSteepnessMinusOne = pow(halfOfSinSplusOne, u_Steepness[i] - 1);
			dhCommon *= halfOfSinSplusOnePowSteepnessMinusOne;
		}

		vec2 dH = vec2(u_Direction[i].x * dhCommon, u_Direction[i].z * dhCommon);
		normal += vec3(-dH.x, -dH.y, 0);
	}		 
	p.y = H;

	//vec3 B = normalize(vec3(1, dH.x, 0));
	//vec3 T = normalize(vec3(0, dH.y, -1));
	//vec3 normal = cross(B, T);

	normal = vec3(normal.x, 1, normal.y);
	normal = normalize(normal);

	gl_Position = p;
	//tes_out.TexUV = tc;
	tes_out.MapIndex = tes_in[0].MapIndex;
	//tes_out.Normal = normalize(vec3(dH.x, 1, -dH.y));
	//tes_out.Tangent = normalize(vec3(0, dH.y, 1));
	//tes_out.Tangent = T;
	tes_out.Normal = normal;
}


