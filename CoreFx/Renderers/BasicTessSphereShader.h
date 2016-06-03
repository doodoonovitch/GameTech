#pragma once

#ifndef COREFX_RENDERERS_BASICTESSSPHERESHADER_H
#define COREFX_RENDERERS_BASICTESSSPHERESHADER_H


namespace CoreFx
{
	namespace Renderers
	{


class BasicTessSphereShader : public Shader
{
public:

	enum EUniformIndex
	{
		u_TessFactor,
		u_DrawColor,

		__uniforms_count__
	};

public:
	BasicTessSphereShader();
	~BasicTessSphereShader();

	virtual void LoadShaders();

private:

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_BASICTESSSPHERESHADER_H