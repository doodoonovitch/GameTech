// ===========================================================================
// ===========================================================================
// ===========================================================================


struct MaterialData
{
	vec3 mBaseColor;
	vec3 mEmissive;

	float mMetallic;
	float mRoughness;
	float mPorosity;

	uint mRendererId;
	uint mEnvMapType;
};

struct FragmentInfo
{
	vec3 mPosition;
	vec3 mNormal;
	vec3 mBaseColor;
	vec3 mEmissive;

	float mMetallic;
	float mRoughness;
	float mPorosity;

	float mDepthNDC;

	uint mRendererId;
	uint mEnvMapType;

	vec3 mDiffuseColor;
	vec3 mSpecularColor;
};

uint Pack4x8(vec4 fValue)
{
	uvec4 uValue = uvec4(fValue * vec4(255));
	uint result = (uValue.x & Mask_0x000000FF) | ((uValue.y & Mask_0x000000FF) << 8) | ((uValue.z & Mask_0x000000FF) << 16) | ((uValue.w & Mask_0x000000FF) << 24);
	return result;
}

vec4 Unpack4x8(uint packedValue)
{
	uvec4 uValue = uvec4(packedValue & Mask_0x000000FF, (packedValue >> 8) & Mask_0x000000FF, (packedValue >> 16) & Mask_0x000000FF, (packedValue >> 24) & Mask_0x000000FF);
	return uValue / vec4(255);
}

// ---------------------------------------------------------------------------
// GBuffer data encoding
//
// ---------------------------------------------------------------------------
void EncodeMaterialData(out uvec4 outUI32Buffer1, MaterialData inData)
{
	uint temp = packUnorm4x8(vec4(0, inData.mMetallic, inData.mRoughness, inData.mPorosity));
	outUI32Buffer1.x = temp | (inData.mRendererId & Mask_0x000000FF);

	//outUI32Buffer1.x = packUnorm4x8(vec4(inData.mRendererId / 255, inData.mMetallic, inData.mRoughness, inData.mPorosity));

	temp = packUnorm4x8(vec4(0, inData.mBaseColor));
	outUI32Buffer1.y = temp | (inData.mEnvMapType & Mask_0x000000FF);

	outUI32Buffer1.z = packHalf2x16(inData.mEmissive.xy);
	outUI32Buffer1.w = packHalf2x16(vec2(0, inData.mEmissive.z));

}

// ---------------------------------------------------------------------------
// GBuffer data decoding
//
// ---------------------------------------------------------------------------
void DecodeMaterialData(out FragmentInfo outData, uvec4 inUI32Buffer1)
{
	outData.mRendererId = inUI32Buffer1.x & Mask_0x000000FF;
	outData.mEnvMapType = inUI32Buffer1.y & Mask_0x000000FF;

	vec4 abcd = unpackUnorm4x8(inUI32Buffer1.x & Mask_0xFFFFFF00);
	//vec4 abcd = unpackUnorm4x8(inUI32Buffer1.x);
	//outData.mRendererId = uint(round(abcd.x * 255));

	outData.mMetallic = abcd.y;
	outData.mRoughness = abcd.z;
	outData.mPorosity = abcd.w;

	outData.mBaseColor = unpackUnorm4x8(inUI32Buffer1.y).yzw;

	outData.mEmissive.xy = unpackHalf2x16(inUI32Buffer1.z);
	vec2 ab = unpackHalf2x16(inUI32Buffer1.w);
	outData.mEmissive.z = ab.y;

}


// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi, in vec2 texUV, in vec2 viewRay, in usampler2D gbufferSampler, in sampler2D depthSampler)
{
	//vec3 n = texture(u_NBufferSampler, texUV, 0).xyz;
	//fi.mNormal = normalize(n);

	uvec4 buffer1Data;

	buffer1Data = texture(gbufferSampler, texUV, 0);
	DecodeMaterialData(fi, buffer1Data);
	
	fi.mDepthNDC = texture(depthSampler, texUV, 0).x * 2.0 - 1.0;
	vec4 posV = vec4(PositionFromDepth(fi.mDepthNDC, viewRay), 1);
	fi.mPosition = (u_InvViewMatrix * posV).xyz;
}

