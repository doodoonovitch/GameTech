#define PI											3.14159265
#define TWO_PI										6.28318531

#define saturate(x) clamp(x, 0, 1);
#define saturate2(v) clamp(v, vec2(0), vec2(1))
#define saturate3(v) clamp(v, vec3(0), vec3(1))
#define saturate4(v) clamp(v, vec4(0), vec4(1))

#define when_eq(a, b) (1 - abs(sign(a - b)))
#define when_ne(a, b) abs(sign(a - b))
#define when_gt(a, b) max(sign(a - b), 0)
#define when_lt(a, b) max(sign(b - a), 0)
#define when_ge(a, b) (1 - when_lt(a, b))
#define when_le(a, b) (1 - when_gt(a, b))




// ===========================================================================
// ===========================================================================
// ===========================================================================




float Square( float x )
{
	return x*x;
}

vec2 Square( vec2 x )
{
	return x*x;
}

vec3 Square( vec3 x )
{
	return x*x;
}

vec4 Square( vec4 x )
{
	return x*x;
}

float Pow2( float x )
{
	return x*x;
}

vec2 Pow2( vec2 x )
{
	return x*x;
}

vec3 Pow2( vec3 x )
{
	return x*x;
}

vec4 Pow2( vec4 x )
{
	return x*x;
}

float Pow3( float x )
{
	return x*x*x;
}

vec2 Pow3( vec2 x )
{
	return x*x*x;
}

vec3 Pow3( vec3 x )
{
	return x*x*x;
}

vec4 Pow3( vec4 x )
{
	return x*x*x;
}

float Pow4( float x )
{
	float xx = x*x;
	return xx * xx;
}

vec2 Pow4( vec2 x )
{
	vec2 xx = x*x;
	return xx * xx;
}

vec3 Pow4( vec3 x )
{
	vec3 xx = x*x;
	return xx * xx;
}

vec4 Pow4( vec4 x )
{
	vec4 xx = x*x;
	return xx * xx;
}

float Pow5( float x )
{
	float xx = x*x;
	return xx * xx * x;
}

vec2 Pow5( vec2 x )
{
	vec2 xx = x*x;
	return xx * xx * x;
}

vec3 Pow5( vec3 x )
{
	vec3 xx = x*x;
	return xx * xx * x;
}

vec4 Pow5( vec4 x )
{
	vec4 xx = x*x;
	return xx * xx * x;
}

float Pow6( float x )
{
	float xx = x*x;
	return xx * xx * xx;
}

vec2 Pow6( vec2 x )
{
	vec2 xx = x*x;
	return xx * xx * xx;
}

vec3 Pow6( vec3 x )
{
	vec3 xx = x*x;
	return xx * xx * xx;
}

vec4 Pow6( vec4 x )
{
	vec4 xx = x*x;
	return xx * xx * xx;
}




// ===========================================================================
// ===========================================================================
// ===========================================================================




struct DualQuat
{
	vec4 Qr;
	vec4 Qd;
};




// ===========================================================================
// ===========================================================================
// ===========================================================================




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

DualQuat dqFromRotationTranslation(in vec4 qRotation, in vec3 translation)
{
	DualQuat d;
	d.Qr = qRotation;
	d.Qd = vec4(0.5f * translation, 0) * qRotation;
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



// ===========================================================================
// ===========================================================================
// ===========================================================================




vec4 qRotationBetweenVectors(in vec3 start, in vec3 dest)
{
	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

	if (cosTheta < (-1 + 0.001f))
	{
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if (length(rotationAxis) < 0.01 ) 
		{	// bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);
		}

		rotationAxis = normalize(rotationAxis);
		return qAngleAxis(PI, rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt( (1 + cosTheta) * 2 );
	float invs = 1 / s;

	return vec4(rotationAxis.xyz * invs, s * 0.5f);
}