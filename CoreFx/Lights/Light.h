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

		Color_Property,
		Intensity_Property,

		__Common_Property_Count__
	};

	enum LightIntensityIndex
	{
		Ambient_Intensity,
		Diffuse_Intensity,
		Specular_Intensity,
	};

	enum LightType
	{
		Point_Light,
		Directional_Light,
		//Spot_Light,

		__light_type_count__
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

	static GLuint GetPropertySize() { return PropertyData::GetPropertySize(); }

	GLuint GetPropertyCount() const { return mPropertyData.GetPropertyCount(); }

	GLuint GetDataSize() const { return mPropertyData.GetDataSize(); }
	const GLfloat * GetData() const { return mPropertyData.GetData(); }

	bool GetIsModified() const { return mPropertyData.GetIsModified(); }

	void SetProperty(const glm::vec4 & value, GLuint propertyIndex);
	void SetProperty(const glm::vec3 & value, GLuint propertyIndex);
	const GLfloat * GetProperty(GLuint propertyIndex) const
	{
		return mPropertyData.GetProperty(propertyIndex);
	}

	void SetColor(const glm::vec3 & value)
	{
		SetProperty(value, Color_Property);
	}
	const glm::vec3 & GetColor() const
	{
		return *(glm::vec3 *)GetProperty(Color_Property);
	}

	void SetIntensity(const glm::vec3 & value)
	{
		SetProperty(value, Intensity_Property);
	}
	const glm::vec3 & GetIntensity() const
	{
		return *(glm::vec3 *)GetProperty(Intensity_Property);
	}

	void SetIntensity(GLfloat ambient, GLfloat diffuse, GLfloat specular)
	{
		glm::vec3 value(ambient, diffuse, specular);
		SetProperty(value, Intensity_Property);
	}
	GLfloat GetAmbientIntensity() const
	{
		return GetProperty(Intensity_Property)[Ambient_Intensity];
	}
	GLfloat GetDiffuseIntensity() const
	{
		return GetProperty(Intensity_Property)[Diffuse_Intensity];
	}
	GLfloat GetSpecularIntensity() const
	{
		return GetProperty(Intensity_Property)[Specular_Intensity];
	}


protected:

	
	Light(LightType lightType, GLuint propertyCount, glm::vec3 const & color, glm::vec3 const & intensity);
	Light(LightType lightType, GLuint propertyCount, glm::vec3 const & color, GLfloat ambientIntensity, GLfloat diffuseIntensity, GLfloat specularIntensity);
	virtual ~Light();

	GLfloat * GetProperty(GLuint propertyIndex) 
	{
		return mPropertyData.GetProperty(propertyIndex);
	}

	void SetIsModified(bool isModified)
	{
		mPropertyData.SetIsModified(isModified);
	}

private:

	void SetDataIndex(GLuint index)
	{
		assert(index <= UINT16_MAX);
		mLightDesc = (mLightDesc & LIGHT_TYPE_MASK) | ((index << LIGHT_DATA_INDEX_SHIFT) & LIGHT_DATA_INDEX_MASK);
	}


private:

	GLuint mLightDesc;
	PropertyData mPropertyData;
};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_LIGHT_H

