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

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DRAWNORMALSHADER_H