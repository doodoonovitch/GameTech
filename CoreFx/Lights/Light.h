#pragma once
#ifndef COREFX_LIGHTS_LIGHT_H
#define COREFX_LIGHTS_LIGHT_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{


class Light : public SceneObject
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Engine;

protected:

	enum
	{
		LIGHT_TYPE_MASK = 0xF,
		LIGHT_DATA_INDEX_SHIFT = 16,
		LIGHT_DATA_INDEX_MASK = 0xFFFF0000
	};

public:

	enum LightType
	{
		Point_Light,
		Directional_Light,
		Spot_Light,
	};
	
	virtual void TransformInViewCoords(const glm::mat4 & viewMatrix) = 0;

	LightType GetLightType() const
	{
		return (LightType)(mLightDesc & LIGHT_TYPE_MASK);
	}

	GLuint GetDataIndex() const
	{
		return mLightDesc >> LIGHT_DATA_INDEX_SHIFT;
	}

	static GLuint GetPropertySize() { return 4 * sizeof(GLfloat); }

	GLuint GetPropertyCount() const { return mPropertyCount; }

	GLuint GetDataSize() const { return mPropertyCount * GetPropertySize(); }
	GLfloat* GetData() const { return mData; }

	bool GetIsModified() const
	{
		return mIsModified;
	}

	void SetProperty(const glm::vec4 & value, GLuint propertyIndex);
	const GLfloat * GetProperty(GLuint propertyIndex) const
	{
		assert(propertyIndex < mPropertyCount);
		return &mData[4 * propertyIndex];
	}


protected:

	
	Light(LightType lightType, GLuint propertyCount);
	virtual ~Light();

	GLfloat * GetProperty(GLuint propertyIndex) 
	{
		assert(propertyIndex < mPropertyCount);
		return &mData[4 * propertyIndex];
	}

	void SetIsModified(bool isModified)
	{
		mIsModified = isModified;
	}

private:

	void SetDataIndex(GLuint index)
	{
		assert(index <= UINT16_MAX);
		mLightDesc = (mLightDesc & LIGHT_TYPE_MASK) | ((index << LIGHT_DATA_INDEX_SHIFT) & LIGHT_DATA_INDEX_MASK);
	}


private:

	GLuint mLightDesc;
	GLfloat * mData;
	GLuint mPropertyCount;
	bool mIsModified;

};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_LIGHT_H

