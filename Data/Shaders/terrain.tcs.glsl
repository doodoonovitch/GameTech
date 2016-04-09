layout (vertices = 4) out;

uniform float u_MinTessDist = 4;
uniform float u_MaxTessDist = 10;
uniform float u_MinTess = 1;
uniform float u_MaxTess = 64;
uniform float u_LODfactor = 40;

in VS_OUT
{
	vec2 TexUV;
	int Layer;
} tcs_in[];

out TCS_OUT
{
	vec2 TexUV;
	int Layer;
} tcs_out[];

bool IsOffScreen(vec4 vertex)
{
    if(vertex.z < -0.5)
	{
        return true;
    }   
    return any(
		lessThan(vertex.xy, vec2(-1.7)) 
		||  greaterThan(vertex.xy, vec2(1.7))
		);  
}

vec2 ScreenSpace(vec4 vertex)
{
    return (clamp(vertex.xy, -1.3, 1.3) + 1) * (u_ScreenSize * 0.5);
}

float TessellationLevel(vec2 v0, vec2 v1)
{
     return clamp(distance(v0, v1) / u_LODfactor, u_MinTess, u_MaxTess);
}

float CompTessFactor(vec4 p1, vec4 p0)
{
	vec2 ss0 = ScreenSpace(p0);
	vec2 ss1 = ScreenSpace(p1);

	return TessellationLevel(ss0, ss1);
}


//float CompTessFactor(vec4 p1, vec4 p0)
//{
//	float d = length((p1.xy - p0.xy) * u_ScreenSize);
//	float s = clamp((d - u_MinTessDist) / (u_MinTessDist - u_MaxTessDist), 0.0, 1.0);
//	//return pow(2, (mix(u_MinTess, u_MaxTess, s)));
//	return mix(u_MinTess, u_MaxTess, s);
//}

void main()
{
	if (gl_InvocationID == 0)
	{
        vec4 p0 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[0].gl_Position.xyz), 1.0);
        vec4 p1 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[1].gl_Position.xyz), 1.0);
        vec4 p2 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[2].gl_Position.xyz), 1.0);
        vec4 p3 = u_ProjMatrix * vec4(dqTransformPoint(u_ViewDQ, gl_in[3].gl_Position.xyz), 1.0);
        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;

        if (p0.z < 0.0 && p1.z < 0.0 && p2.z < 0.0 && p3.z < 0.0)
		{
            gl_TessLevelOuter[0] = 0.0;
            gl_TessLevelOuter[1] = 0.0;
            gl_TessLevelOuter[2] = 0.0;
            gl_TessLevelOuter[3] = 0.0;
        }
		else
		{
            float l0 = CompTessFactor(p2, p0); 
            float l1 = CompTessFactor(p3, p2); 
            float l2 = CompTessFactor(p3, p1); 
            float l3 = CompTessFactor(p1, p0); 
            gl_TessLevelOuter[0] = l0;
            gl_TessLevelOuter[1] = l1;
            gl_TessLevelOuter[2] = l2;
            gl_TessLevelOuter[3] = l3;
            gl_TessLevelInner[0] = min(l1, l3);
            gl_TessLevelInner[1] = min(l0, l2);
        }
	} 

    tcs_out[gl_InvocationID].TexUV = tcs_in[gl_InvocationID].TexUV;
    tcs_out[gl_InvocationID].Layer = tcs_in[gl_InvocationID].Layer;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


