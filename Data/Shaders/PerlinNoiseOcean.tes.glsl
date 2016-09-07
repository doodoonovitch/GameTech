//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;

const int c_MaxWavesToSum = 4;

uniform vec3[c_MaxWavesToSum] u_Direction;
uniform float[c_MaxWavesToSum] u_WaveLength;
uniform float[c_MaxWavesToSum] u_Amplitude;
uniform float[c_MaxWavesToSum] u_Velocity;

uniform sampler2D u_noiseHeightSampler;
uniform sampler2D u_noiseNormalSampler;

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

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);

    mat3 TBN = mat3(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, 0, 1));

	float t = u_TimeDeltaTime.x;
	float H = 0;
	vec3 normal  = vec3(0);
	for(int i = 0; i < 4; ++i)
	{
		
		//vec2 uv = tc * u_WaveLength[i] / coef + t * u_Velocity[i] * u_Direction[0].xz;
		vec2 uv = tc * u_WaveLength[i] + t * u_Velocity[i] * u_Direction[i].xz;

		H += u_Amplitude[i] * texture(u_noiseHeightSampler, uv).r;

		vec3 bumpMapNormal = texture(u_noiseNormalSampler, uv).xzy;
		bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
		vec3 newNormal = /*TBN * */normalize(bumpMapNormal); 
		newNormal.y = newNormal.y * u_Amplitude[i];
		normal += newNormal;
	}		 
	p.y = H;

	vec4 viewPos = u_ViewMatrix * p;

	tes_out.Position = p.xyz;
	tes_out.ViewPosition = viewPos.xyz;
	tes_out.TexUV = tc;
	tes_out.MapIndex = tes_in[0].MapIndex;
	tes_out.Normal = normalize(normal);
	
	gl_Position = u_ProjMatrix * viewPos;
}


