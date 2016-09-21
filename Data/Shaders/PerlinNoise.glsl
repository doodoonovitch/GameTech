uniform sampler1D u_PerlinNoisePermSampler;
uniform sampler2D u_PerlinNoisePerm2DSampler;
uniform sampler1D u_PerlinNoiseGradSampler;
uniform sampler1D u_PerlinNoisePermGradSampler;
uniform sampler1D u_PerlinNoisePermGrad4DSampler;
uniform sampler1D u_PerlinNoiseGrad4DSampler;

vec3 pnFade(vec3 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10); 
}

vec4 pnFade4D(vec4 t)
{
	return t * t * t * (t * (t * 6 - 15) + 10); 
}

float pnPerm(float x)
{
	return texture(u_PerlinNoisePermSampler, x).x;
}

vec4 pnPerm2D(vec2 p)
{
	return texture(u_PerlinNoisePerm2DSampler, p);
}

float pnGrad(float x, vec3 p)
{
	return dot(texture(u_PerlinNoiseGradSampler, x * 16).xyz, p).x;
}

float pnGradPerm(float x, vec3 p)
{
	return dot(texture(u_PerlinNoisePermGradSampler, x).xyz, p);
}

// 4d versions
float pnGrad4D(float x, vec4 p)
{
	return dot(texture(u_PerlinNoiseGrad4DSampler, x), p);
}

float pnGradPerm4D(float x, vec4 p)
{
	return dot(texture(u_PerlinNoisePermGrad4DSampler, x), p);
}

float pnNoise(vec3 p)
{
	vec3 P = mod(floor(p), 256.0);	// FIND UNIT CUBE THAT CONTAINS POINT
  	p -= floor(p);                      // FIND RELATIVE X,Y,Z OF POINT IN CUBE.
	vec3 f = pnFade(p);                 // COMPUTE FADE CURVES FOR EACH OF X,Y,Z.

	P = P / 256.0;
	const float one = 1.0 / 256.0;
	
    // HASH COORDINATES OF THE 8 CUBE CORNERS
  	float A = pnPerm(P.x) + P.y;
  	float4 AA;
  	AA.x = pnPerm(A) + P.z;
	AA.y = pnPerm(A + one) + P.z;
  	float B =  pnPerm(P.x + one) + P.y;
  	AA.z = pnPerm(B) + P.z;
  	AA.w = pnPerm(B + one) + P.z;
 
	// AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
  	return mix( mix( mix( pnGrad(pnPerm(AA.x), p ),  
                             pnGrad(pnPerm(AA.z), p + vec3(-1, 0, 0) ), f.x),
                       mix( pnGrad(pnPerm(AA.y), p + vec3(0, -1, 0) ),
                             pnGrad(pnPerm(AA.w), p + vec3(-1, -1, 0) ), f.x), f.y),
                             
                 mix( mix( pnGrad(pnPerm(AA.x + one), p + vec3(0, 0, -1) ),
                             pnGrad(pnPerm(AA.z + one), p + vec3(-1, 0, -1) ), f.x),
                       mix( pnGrad(pnPerm(AA.y + one), p + vec3(0, -1, -1) ),
                             pnGrad(pnPerm(AA.w + one), p + vec3(-1, -1, -1) ), f.x), f.y), f.z);
}
