#pragma once
#ifndef COREFX_GEOMETRY_VERTEXDATA_H
#define COREFX_GEOMETRY_VERTEXDATA_H

#include <glm\glm.hpp>
#include <GL\glew.h>

namespace CoreFx
{
	namespace Geometry
	{


class Vertex
{
public:

	glm::vec3 mPosition;
};

class VertexN
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
};

class VertexNT
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec3 mTangent;
};

class VertexNTB
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec3 mTangent;
	glm::vec3 mBinormal;
};

class VertexUV
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec2 mTexUV;
};

class VertexNUV
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec2 mTexUV;
};

class VertexNTUV
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec3 mTangent;
	glm::vec2 mTexUV;
};

class VertexNTBUV
{
public:

	glm::vec3 mPosition;
	glm::vec3 mNormal;
	glm::vec2 mTexUV;
	glm::vec3 mTangent;
	glm::vec3 mBinormal;
};






template<typename TVertex, typename TIndex>
class VertexBufferT
{
public:

	VertexBufferT()
	{}

	VertexBufferT(GLsizei vertexCount, GLsizei indexCount)
		: mVertexBuffer(vertexCount)
		, mIndexBuffer(indexCount)
	{}

	inline GLsizei GetVertexCount() const
	{
		return (GLsizei)mVertexBuffer.size();
	}

	inline GLsizei GetIndexCount() const
	{
		return (GLsizei)mIndexBuffer.size();
	}

	inline TVertex* GetVertexBuffer()
	{
		return mVertexBuffer.data();
	}

	inline const TVertex* GetVertexBuffer() const
	{
		return mVertexBuffer.data();
	}

	inline TIndex* GetIndexBuffer()
	{
		return mIndexBuffer.data();
	}

	inline const TIndex* GetIndexBuffer() const
	{
		return mIndexBuffer.data();
	}

	void ResizeVertexBuffer(GLsizei vertexCount)
	{
		mVertexBuffer.resize(vertexCount);
	}

	void ResizeIndexBuffer(GLsizei indexCount)
	{
		mIndexBuffer.resize(vertexCount);
	}

protected:

	std::vector<TVertex> mVertexBuffer;
	std::vector<TIndex> mIndexBuffer;

};

	} // namespace Geometry
} // namespace Core
#endif // COREFX_GEOMETRY_VERTEXDATA_H