#version 420

#define	POSITION_ATTRIBUTE							0
#define	NORMAL_ATTRIBUTE							1
#define	TANGENT_ATTRIBUTE							2
#define	BINORMAL_ATTRIBUTE							3
#define	UV_ATTRIBUTE								4
#define	MATERIALID_ATTRIBUTE						5

#define MAX_LIGHT_COUNT								16

#define POINT_LIGHT_TYPE							0
#define DIRECTIONAL_LIGHT_TYPE						1
#define SPOT_LIGHT_TYPE								2

#define POINT_LIGHT_COLOR_INDEX						0	
#define POINT_LIGHT_POSITION_INDEX					1	

#define DIRECTIONAL_LIGHT_COLOR_INDEX				0	
#define DIRECTIONAL_LIGHT_DIRECTION_INDEX			1	


#define PI											3.14159265

#define GetLightDesc(index)							u_LightDesc[index / 4][index & 3]
#define GetLightType(lightDesc)						(lightDesc & 15)
#define GetLightDataIndex(lightDesc)				(lightDesc >> 16)

/*
#define GetAmbientTextureIndex(bitfieldValue)		(bitfieldValue & 0x000000FF)
#define GetDiffuseTextureIndex(bitfieldValue)		((bitfieldValue & 0xFF000000) >> 24)
#define GetSpecularTextureIndex(bitfieldValue)		((bitfieldValue & 0x00FF0000) >> 16)
#define GetNormalTextureIndex(bitfieldValue)		((bitfieldValue & 0x0000FF00) >> 8)
*/

#define GetAmbientTextureIndex(bitfieldValue)		(bitfieldValue & uint(255))
#define GetDiffuseTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 24)) >> 24)
#define GetSpecularTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 16)) >> 16)
#define GetNormalTextureIndex(bitfieldValue)		((bitfieldValue & uint(255 << 8)) >> 8)


struct DualQuat
{
	vec4 Qr;
	vec4 Qd;
};


layout (std140, shared) uniform FrameData
{
	mat4 u_ProjMatrix;

	DualQuat u_ViewDQ;

	vec4 u_AmbientLight;

		// bits 16 - 31 (16 bits) : index in the light data texture buffer
		// bits 0 - 3 (4 bits) : light type
		// bits 4 - 15 : reserved 
	ivec4 u_LightDesc[MAX_LIGHT_COUNT / 4]; 
	vec4 u_LightData[64];
	int u_LightCount; 
};




// ===========================================================================
// ===========================================================================
// ===========================================================================




#define when_eq(a, b)								(1 - abs(sign(a - b)))
#define when_ne(a, b)								abs(sign(a - b))
#define when_gt(a, b)								max(sign(a - b), 0)
#define when_lt(a, b)								max(sign(b - a), 0)
#define when_ge(a, b)								(1 - when_lt(a, b))
#define when_le(a, b)								(1 - when_gt(a, b))


#define qId											vec4(0, 0, 0, 1)
#define qLength(q)									length(q.xyzw)
#define qConjugate(q)								vec4(-q.xyz, q.w)
#define qInverse(q)									(qConjugate(q) / qLength(q))
#define qAdd(q1, q2)								(q1.xyzw + q2.xyzw)
#define qMul(q1, q2)								vec4( q1.w * q2.xyz + q2.w * q1.xyz + cross(q1.xyz, q2.xyz), q1.w * q2.w - dot(q1.xyz, q2.xyz) )
#define qDot(q1, q2)								dot(q1.xyzw, q2.xyzw)
#define qTransform(q, inv_q, point)					qMul(qMul(q, vec4(p.xyz, 0)), inv_q)

vec4 qAngleAxis(float angle, vec3 axis)
{
	float halfAngle = angle * 0.5f;
	float s = sin(halfAngle);
	vec4 result = vec4(axis.xyz * s, cos(halfAngle));
	return result;
}

DualQuat dqFromRotation(in vec4 qRotation)
{
	DualQuat d;
	d.Qr = qRotation;
	d.Qd = vec4(0);
	return d;
}

DualQuat dqFromTranslation(in vec3 translation)
{
	DualQuat d;
	d.Qr = qId;
	d.Qd = vec4(translation.xyz * 0.5f, 0);
	return d;
}

DualQuat dqConjugate(in DualQuat q)
{
	DualQuat result;
	result.Qr = qConjugate(q.Qr);
	result.Qd = qConjugate(q.Qd);
	return result;
}

DualQuat dqAdd(in DualQuat a, in DualQuat b)
{
	DualQuat result;
	result.Qr = a.Qr + b.Qr;
	result.Qd = a.Qd + b.Qd;
	return result;
}

DualQuat dqMul(in DualQuat a, in DualQuat b)
{
	DualQuat result;
	result.Qr = qMul(a.Qr, b.Qr); 
	result.Qd = qMul(a.Qr, b.Qd) + qMul(a.Qd, b.Qr);
	return result;
}

DualQuat dqMul(in DualQuat q, in float s)
{
	DualQuat result;
	result.Qr = s * q.Qr;
	result.Qd = s * q.Qd;
	return result;
}

DualQuat dqMul(in float s, in DualQuat q)
{
	DualQuat result;
	result.Qr = s * q.Qr;
	result.Qd = s * q.Qd;
	return result;
}

DualQuat dqNormalize(in DualQuat q)
{
	float mag = qDot(q.Qr, q.Qr);
	DualQuat result;
	result.Qr = q.Qr /mag;
	result.Qd = q.Qd /mag;
	return result;
}

float dqDot(in DualQuat a, in DualQuat b)
{
	return qDot(a.Qr, b.Qr);
}

#define dqGetRotation(q)						q.Qr
#define dqGetTranslation(q)						qMul(q.Qd * 2.0f, qConjugate(q.Qr)).xyz

vec3 dqTransformPoint(DualQuat dq, vec3 pt )
{
	return pt + 2 * cross( dq.Qr.xyz, cross( dq.Qr.xyz, pt ) + dq.Qr.w * pt )
		+ 2 * ( dq.Qr.w * dq.Qd.xyz - dq.Qd.w * dq.Qr.xyz + cross( dq.Qr.xyz, dq.Qd.xyz ) );
}

vec3 dqTransformNormal(vec3 normal, DualQuat dq)
{
    return normal + 2.0 * cross( dq.Qr.xyz, cross( dq.Qr.xyz, normal ) + dq.Qr.w * normal );
}
