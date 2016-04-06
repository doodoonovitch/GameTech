#version 420

// ---------------------------------------------------------------------------
// Vertex attribute locations
//
#define	POSITION_ATTRIBUTE							0
#define	NORMAL_ATTRIBUTE							1
#define	TANGENT_ATTRIBUTE							2
#define	UV_ATTRIBUTE								3
#define	MATERIALID_ATTRIBUTE						4
#define	BINORMAL_ATTRIBUTE							5
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Light property indices
//
#define POINT_LIGHT_TYPE							0
#define DIRECTIONAL_LIGHT_TYPE						1
#define SPOT_LIGHT_TYPE								2

#define LIGHT_COLOR_PROPERTY						0	

#define POINT_LIGHT_POSITION_PROPERTY				1
#define POINT_LIGHT_ATTENUATION_PROPERTY			2	

#define SPOT_LIGHT_POSITION_PROPERTY				1
#define SPOT_LIGHT_DIRECTION_PROPERTY				2
#define SPOT_LIGHT_ATTENUATION_PROPERTY				3	

#define DIRECTIONAL_LIGHT_DIRECTION_PROPERTY		1
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Light helper macros
//
#define GetLightType(lightDesc)						(lightDesc & 15)
#define GetLightDataIndex(lightDesc)				(lightDesc >> 16)
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Material sampler first index and max sampler count
//
#define FIRST_TEXTURE_SAMPLER_INDEX					10
#define MAX_TEXTURE_SAMPLER							32
//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Render ids
//
#define CUBE_RENDERER_ID							1
#define AXIS_RENDERER_ID							2
#define GRID_RENDERER_ID							3
#define VERTEX_NORMAL_RENDERER_ID					4
#define TERRAIN_RENDERER_ID							5
//
// ---------------------------------------------------------------------------

#define Mask_0x00FFFFFF								uint(16777215)
#define Mask_0xFF000000								uint(uint(255) << 24)
#define CombineRenderIdAndMaterialIndex(rendererId, materialIndex) uint((rendererId << 24) | (materialIndex & Mask_0x00FFFFFF))


#define PI											3.14159265


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

	int u_PointLightCount; 
	int u_SpotLightCount;
	int u_DirectionalLightCount;
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

vec3 GetBumpedNormal(sampler2DArray gNormalMap, vec3 texCoord)
{
    vec3 bumpMapNormal = texture(gNormalMap, texCoord).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
	return bumpMapNormal;
}

vec3 ComputeBumpedNormal(vec3 normal, vec3 tangent, vec3 bumpMapNormal)
{
	normal = normalize(normal);
	tangent = normalize(tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 newNormal;
    newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

vec3 ComputeBumpedNormal(vec3 normal, vec3 tangent, sampler2DArray gNormalMap, vec3 texCoord)
{
	vec3 bumpMapNormal = GetBumpedNormal(gNormalMap, texCoord);
	return ComputeBumpedNormal(normal, tangent, bumpMapNormal);
}

