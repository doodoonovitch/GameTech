layout (vertices = 4) out;

uniform float u_MaxDist = 700;
uniform float u_MinDist = 5;
uniform float u_MinTess = 1;
uniform float u_MaxTess = 6;
uniform int u_MinMipmap = 0;
uniform int u_MaxMipmap = 9;


in VS_OUT
{
	vec2 TexUV;
	flat int MapIndex;
} tcs_in[];

out TCS_OUT
{
	vec2 TexUV;
	//float MipmapLevel;
	flat int MapIndex;
} tcs_out[];

float CompTessFactor(float d)
{
	float s = clamp((d - u_MinDist) / (u_MaxDist - u_MinDist), 0, 1);
	return pow(2, round(mix(u_MaxTess, u_MinTess, s)));
}

float CompMipmapLevel(float d)
{
	float s = clamp((d - u_MinDist) / (u_MaxDist - u_MinDist), 0, 1);
	return mix(u_MaxMipmap, u_MinMipmap, s);
}

void main()
{
	if (gl_InvocationID == 0)
	{
		vec4 p[4];
		p[0] = u_ViewProjMatrix * gl_in[0].gl_Position;
		p[1] = u_ViewProjMatrix * gl_in[1].gl_Position;
		p[2] = u_ViewProjMatrix * gl_in[2].gl_Position;
		p[3] = u_ViewProjMatrix * gl_in[3].gl_Position;
		for(int i = 0; i < 4; ++i)
			p[i] /= p[i].w;

        if (p[0].z < 0.0 && p[1].z < 0.0 && p[2].z < 0.0 && p[3].z < 0.0)
		{
            gl_TessLevelOuter[0] = 0.0;
            gl_TessLevelOuter[1] = 0.0;
            gl_TessLevelOuter[2] = 0.0;
            gl_TessLevelOuter[3] = 0.0;
        }
		else
		{
			vec4 dist;
			dist[0] = distance(((gl_in[2].gl_Position.xz - gl_in[0].gl_Position.xz) * 0.5), u_ViewPosition.xz);
			dist[1] = distance(((gl_in[3].gl_Position.xz - gl_in[2].gl_Position.xz) * 0.5), u_ViewPosition.xz);
			dist[2] = distance(((gl_in[3].gl_Position.xz - gl_in[1].gl_Position.xz) * 0.5), u_ViewPosition.xz);
			dist[3] = distance(((gl_in[1].gl_Position.xz - gl_in[0].gl_Position.xz) * 0.5), u_ViewPosition.xz);	

			vec4 tessLevel;
			float centerDist = 0;
			for(int i = 0; i < 4; ++i)
			{
				tessLevel[i] = CompTessFactor(dist[i]);
				gl_TessLevelOuter[i] = tessLevel[i];
				centerDist += dist[i];
            }

			gl_TessLevelInner[0] = CompTessFactor(centerDist * 0.25);
			gl_TessLevelInner[1] = gl_TessLevelInner[0];
            //gl_TessLevelInner[0] = min(tessLevel[1], tessLevel[3]);
            //gl_TessLevelInner[1] = min(tessLevel[0], tessLevel[2]);

			//tcs_out[gl_InvocationID].MipmapLevel = CompMipmapLevel((dist[0] + dist[1] + dist[2] + dist[3]) * 0.25);
        }
	} 

    tcs_out[gl_InvocationID].TexUV = tcs_in[gl_InvocationID].TexUV;
    tcs_out[gl_InvocationID].MapIndex = tcs_in[gl_InvocationID].MapIndex;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


