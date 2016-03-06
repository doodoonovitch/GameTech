#pragma once
#ifndef COREFX_GEOMETRY_MATERIALDATA_H
#define COREFX_GEOMETRY_MATERIALDATA_H

#include <glm\glm.hpp>


namespace CoreFx
{
	namespace Geometry
	{


class MaterialData
{
public:
	MaterialData();
	~MaterialData();

	glm::vec4 mAmbient;
	glm::vec4 mDiffuse;
	glm::vec4 mEmissive;
	glm::vec4 mReflection;
	glm::vec4 mSpecular;
	glm::vec4 mNormalMap;

	GLfloat mTransparency;
	GLfloat mShininess;

	enum
	{
		TEXTURE_INDEX_BIT_MASK = 0x1FF,

		SAMPLER_INDEX_SHIFT = 9,
		SAMPLER_INDEX_BIT_MASK = (0x1F << SAMPLER_INDEX_SHIFT),

		SWAP_UV_SHIFT = 14,
		SWAP_UV_BIT_MASK = (1 << SWAP_UV_SHIFT),
	};

	// texture index (9 bits : 0 to 8) + texture sample index (bit 9 to 13) + swap UV (1 bit 14) 
	int mNormalTexture;				
	int mAbientTexture;
	int mDiffuseTexture;
	int mEmissiveTexture;
	int mReflectionTexture;
	int mSpecularTexture;
};

class TextureDesc
{
public:

	std::string mFilename;
	int mSwapUV;
	int mSamplerIndex;
};

class SamplerDesc
{
public:

	SamplerDesc() {}

	SamplerDesc(int wrapU, int wrapV, int index)
		: mWrapU(wrapU)
		, mWrapV(wrapV)
		, mIndex(index)
	{}

	int mWrapU;
	int mWrapV;
	int mIndex;
};

	} // namespace Geometry
} // namespace Core
#endif // COREFX_GEOMETRY_MATERIALDATA_H