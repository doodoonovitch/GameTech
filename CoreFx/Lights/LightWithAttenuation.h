#pragma once
#ifndef COREFX_LIGHTS_LIGHTWITHATTENUATION_H
#define COREFX_LIGHTS_LIGHTWITHATTENUATION_H


namespace CoreFx
{

	class Engine;

	namespace Lights
	{



template<GLuint T_Attenuation_Property_Index>
class LightWithAttenuation : public Light
{
public:

	GLfloat GetConstantAttenuation() const
	{
		return GetProperty(T_Attenuation_Property_Index)[0];
	}

	GLfloat GetLinearAttenuation() const
	{
		return GetProperty(T_Attenuation_Property_Index)[1];
	}

	GLfloat GetQuadraticAttenuation() const
	{
		return GetProperty(T_Attenuation_Property_Index)[2];
	}


	void SetConstantAttenuation(GLfloat value) const
	{
		GetProperty(T_Attenuation_Property_Index)[0] = value;
		SetIsModified(true);
	}

	void SetLinearAttenuation(GLfloat value) const
	{
		GetProperty(T_Attenuation_Property_Index)[1] = value;
		SetIsModified(true);
	}

	void SetQuadraticAttenuation(GLfloat value) const
	{
		GetProperty(T_Attenuation_Property_Index)[2] = value;
		SetIsModified(true);
	}

protected:

	LightWithAttenuation(LightType lightType, GLuint propertyCount, GLfloat constantAttenuation, GLfloat linearAttenuation, GLfloat quadraticAttenuation)
		: Light(lightType, propertyCount)
	{
		SetProperty(glm::vec4(constantAttenuation, linearAttenuation, quadraticAttenuation, 0.f), T_Attenuation_Property_Index);
	}

	virtual ~LightWithAttenuation()
	{

	}

};


	} // Namespace Lights
} // namespace CoreFx
#endif // COREFX_LIGHTS_LIGHTWITHATTENUATION_H

