#pragma once

#ifndef COREFX_RENDERERS_DRAWNORMALSHADER_H
#define COREFX_RENDERERS_DRAWNORMALSHADER_H


namespace CoreFx
{
	namespace Renderers
	{


class DrawNormalShader : public Shader
{
public:
	DrawNormalShader();
	~DrawNormalShader();

	void LoadShaders();

	void SetUniformValues() const;

private:

	enum EAttributeIndex
	{
		in_Position,
		in_Normal,

		__attributes_count__
	};

	enum EUniformIndex
	{
		perInstanceDataSampler,

		__uniforms_count__
	};


};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DRAWNORMALSHADER_H