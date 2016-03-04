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

	enum BasicLightPropertyIndex
	{

		Ambient_Color_Property,
		Diffuse_Color_Property,
		Specular_Color_Property,

		__Ambient_Diffuse_Specular_Color_Property_Count__
	};

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
	void SetProperty(const glm::vec3 & value, GLuint propertyIndex);
	const GLfloat * GetProperty(GLuint propertyIndex) const
	{
		assert(propertyIndex < mPropertyCount);
		return &mData[4 * propertyIndex];
	}

	void SetAmbient(const glm::vec3 & value)
	{
		SetProperty(value, Ambient_Color_Property);
	}
	const glm::vec3 & GetAmbient() const
	{
		return *(glm::vec3 *)GetProperty(Ambient_Color_Property);
	}

	void SetDiffuse(const glm::vec3 & value)
	{
		SetProperty(value, Diffuse_Color_Property);
	}
	const glm::vec3 & GetDiffuse() const
	{
		return *(glm::vec3 *)GetProperty(Diffuse_Color_Property);
	}

	void SetSpecular(const glm::vec3 & value)
	{
		SetProperty(value, Specular_Color_Property);
	}
	const glm::vec3 & GetSpecular() const
	{
		return *(glm::vec3 *)GetProperty(Specular_Color_Property);
	}


protected:

	
	Light(LightType lightType, GLuint propertyCount, glm::vec3 const & ambient, glm::vec3 const & diffuse, glm::vec3 const & specular);
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

