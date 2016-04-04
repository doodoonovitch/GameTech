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
		u_NormalMagnitude,
		u_LightMagnitude,
		u_VertexNormalColor,
		u_PointLightColor,
		u_DirectionalLightColor,
		u_FirstLightIndex,
		u_DrawLightCount,
		perInstanceDataSampler,
		lightDescSampler,
		lightDataSampler,

		__uniforms_count__
	};


};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DRAWNORMALSHADER_H