#pragma once

#ifndef COREFX_RENDERERS_DEBUGTOOLS_DRAWGBUFFERNORMALSHADER_H
#define COREFX_RENDERERS_DEBUGTOOLS_DRAWGBUFFERNORMALSHADER_H


namespace CoreFx
{
	namespace Renderers
	{


class DrawGBufferNormalShader : public Shader
{
public:

	enum EUniformIndex
	{
		u_PatchCount,
		u_gDepthMap,
		u_gBufferNormal,

		__uniforms_count__
	};

public:
	DrawGBufferNormalShader();
	~DrawGBufferNormalShader();

	void LoadShaders();

	void SetUniformValues() const;

private:

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEBUGTOOLS_DRAWGBUFFERNORMALSHADER_H