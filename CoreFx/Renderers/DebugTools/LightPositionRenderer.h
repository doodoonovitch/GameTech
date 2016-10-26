#pragma once

#ifndef COREFX_RENDERERS_DEBUGTOOLS_LIGHTPOSITIONRENDERER_H
#define COREFX_RENDERERS_DEBUGTOOLS_LIGHTPOSITIONRENDERER_H


namespace CoreFx
{
	class Engine;

	namespace Renderers
	{


class PointLightPositionRenderer : IcosahedronRendererBase
{
	friend class Engine;

public:
	PointLightPositionRenderer();
	virtual ~PointLightPositionRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

protected:


	enum EUniformIndex
	{
		u_InnerTessLevel,
		u_OuterTessLevel,
		u_LightDescSampler,
		u_LightDataSampler,
		u_LightOffset,

		__uniforms_count__
	};


	void InitializeUniforms(Shader & shader);
	void InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId);

};




class SpotLightPositionRenderer : ConeRendererBase
{
	friend class Engine;

public:
	SpotLightPositionRenderer(GLuint numStrips = 18);
	virtual ~SpotLightPositionRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

protected:


	enum EUniformIndex
	{
		u_LightDescSampler,
		u_LightDataSampler,
		u_LightOffset,

		__uniforms_count__
	};


	void InitializeUniforms(Shader & shader);
	void InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId);
};




	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_DEBUGTOOLS_LIGHTPOSITIONRENDERER_H