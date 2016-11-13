layout (vertices = 4) out;

uniform float u_MinTessDist = 600;
uniform float u_MaxTessDist = 5;
uniform float u_MinTess = 1;
uniform float u_MaxTess = 6;
//uniform float u_LODfactor = 40;

//uniform samplerBuffer u_PerMapDataSampler;
layout (std430, binding = 1) buffer PerInstanceViewMatrix
{
	vec4 data[];
} u_PerInstanceViewMatrix;


in VS_OUT
{
	vec2 TexUV;
	int MapIndex;
} tcs_in[];

out TCS_OUT
{
	vec2 TexUV;
	int MapIndex;
} tcs_out[];


float CompTessFactor(vec4 p1, vec4 p0)
{
	vec3 p = (p0.xyz + p1.xyz) * 0.5;
	p = p - u_ViewPosition.xyz;
	float d = length(p);
	float diffTess = u_MaxTess - u_MinTess;
	float diffDist = u_MinTessDist - u_MaxTessDist;
	return pow(2, clamp(round(diffTess * (1 - (d - u_MinTess)/diffDist) + u_MinTess), u_MinTess, u_MaxTess));
}


void CompMinMaxPoint(vec4 p[4], out vec4 minima, out vec4 maxima)
{
	minima = p[0];
	maxima = p[0];
	for(int i = 1; i < 4; ++i)
	{
		if(p[i].x < minima.x)
			minima.x = p[i].x;
		else if(p[i].x > maxima.x)
			maxima.x = p[i].x;

		if(p[i].y < minima.y)
			minima.y = p[i].y;
		else if(p[i].y > maxima.y)
			maxima.y = p[i].y;

		if(p[i].z < minima.z)
			minima.z = p[i].z;
		else if(p[i].z > maxima.z)
			maxima.z = p[i].z;
	}
}

#define XMinFrustum -1.3
#define XMaxFrustum 1.3

#define YMinFrustum -1.3
#define YMaxFrustum 1.3

#define ZMinFrustum -0.3
#define ZMaxFrustum 1.3

bool IsOffScreen(vec4 minima, vec4 maxima)
{
	if (minima.x > XMaxFrustum || XMinFrustum > maxima.x)
		return true;
		
	if (minima.y > YMaxFrustum || YMinFrustum > maxima.y)
		return true;

	if (minima.z > ZMaxFrustum || ZMinFrustum > maxima.z)
		return true;

	return false;
}

void main()
{
	if (gl_InvocationID == 0)
	{
		int index = tcs_in[gl_InvocationID].MapIndex * 2;
		//DualQuat modelDQ;
		//modelDQ.Qr = texelFetch(u_PerMapDataSampler, index);
		//modelDQ.Qd = texelFetch(u_PerMapDataSampler, index + 1);
		//DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);

		DualQuat viewModelDQ;
		viewModelDQ.Qr = u_PerInstanceViewMatrix.data[index];
		viewModelDQ.Qd = u_PerInstanceViewMatrix.data[index + 1];


		vec4 p[4];
		p[0] = u_ProjMatrix * vec4(dqTransformPoint(viewModelDQ, gl_in[0].gl_Position.xyz), 1.0);
		p[1] = u_ProjMatrix * vec4(dqTransformPoint(viewModelDQ, gl_in[1].gl_Position.xyz), 1.0);
		p[2] = u_ProjMatrix * vec4(dqTransformPoint(viewModelDQ, gl_in[2].gl_Position.xyz), 1.0);
		p[3] = u_ProjMatrix * vec4(dqTransformPoint(viewModelDQ, gl_in[3].gl_Position.xyz), 1.0);	
		for(int i = 0; i < 4; ++i)
			p[i] /= p[i].w;
		//vec4 minima, maxima;
		//CompMinMaxPoint(p, minima, maxima);
		//if (IsOffScreen(minima, maxima))

        if (p[0].z < 0.0 && p[1].z < 0.0 && p[2].z < 0.0 && p[3].z < 0.0)
		{
            gl_TessLevelOuter[0] = 0.0;
            gl_TessLevelOuter[1] = 0.0;
            gl_TessLevelOuter[2] = 0.0;
            gl_TessLevelOuter[3] = 0.0;
        }
		else
		{
			p[0] = gl_in[0].gl_Position;
			p[1] = gl_in[1].gl_Position;
			p[2] = gl_in[2].gl_Position;
			p[3] = gl_in[3].gl_Position;	

            float l0 = CompTessFactor(p[2], p[0]); 
            float l1 = CompTessFactor(p[3], p[2]); 
            float l2 = CompTessFactor(p[3], p[1]); 
            float l3 = CompTessFactor(p[1], p[0]); 
            gl_TessLevelOuter[0] = l0;
            gl_TessLevelOuter[1] = l1;
            gl_TessLevelOuter[2] = l2;
            gl_TessLevelOuter[3] = l3;
            gl_TessLevelInner[0] = min(l1, l3);
            gl_TessLevelInner[1] = min(l0, l2);
        }
	} 

    tcs_out[gl_InvocationID].TexUV = tcs_in[gl_InvocationID].TexUV;
    tcs_out[gl_InvocationID].MapIndex = tcs_in[gl_InvocationID].MapIndex;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}


