#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{




PointLightPositionRenderer::PointLightPositionRenderer()
	: Renderers::IcosahedronRendererBase()
{
	InitializeVertexBuffer();
	LoadShaders(mShader, "shaders/IcosahedronShader.PointLight.gs.glsl", "shaders/IcosahedronShader.PointLight.forward.fs.glsl");
	InitializeUniforms(mShader);
	LoadShaders(mWireFrameShader, "shaders/IcosahedronShader.PointLight.gs.glsl", "shaders/IcosahedronShader.WireFrame.forward.fs.glsl");
	InitializeUniforms(mWireFrameShader);
	mIsInitialized = true;
}

PointLightPositionRenderer::~PointLightPositionRenderer()
{

}

void PointLightPositionRenderer::InitializeUniforms(Shader & shader)
{
	const char * uniformNames[__uniforms_count__] =
	{
		"u_InnerTessLevel",
		"u_OuterTessLevel",
		"u_LightDescSampler",
		"u_LightDataSampler",
		"u_LightOffset",
	};

	shader.Use();
	shader.AddUniforms(uniformNames, __uniforms_count__);

	glUniform1i(shader.GetUniform(u_LightDescSampler), 0);
	glUniform1i(shader.GetUniform(u_LightDataSampler), 1);
	glUniform1f(shader.GetUniform(u_InnerTessLevel), 1.f);
	glUniform1f(shader.GetUniform(u_OuterTessLevel), 1.f);

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;
}

void PointLightPositionRenderer::InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId)
{
	glDisable(GL_CULL_FACE);
	shader.Use();
	glBindVertexArray(mVaoID);

	glUniform1i(shader.GetUniform(u_LightOffset), lightOffset);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, lightDescBufferId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, lightDataBufferId);

	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElementsInstanced(GL_PATCHES, mIndexCount, GL_UNSIGNED_SHORT, 0, instanceCount);

	glBindVertexArray(0);
	shader.UnUse();
	glEnable(GL_CULL_FACE);
}

void PointLightPositionRenderer::Render()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Point_Light];
	InternalRender(mShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Point_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}

void PointLightPositionRenderer::RenderWireFrame()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Point_Light];
	InternalRender(mWireFrameShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Point_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}





// =======================================================================
// =======================================================================
// =======================================================================




SpotLightPositionRenderer::SpotLightPositionRenderer(GLuint numStrips)
	: Renderers::ConeRendererBase()
{
	InitializeVertexBuffer(numStrips);
	LoadShaders(mShader, "shaders/ConeShader.Light.gs.glsl", "shaders/ConeShader.Light.forward.fs.glsl");
	InitializeUniforms(mShader);
	LoadShaders(mWireFrameShader, "shaders/ConeShader.Light.gs.glsl", "shaders/IcosahedronShader.WireFrame.forward.fs.glsl");
	InitializeUniforms(mWireFrameShader);

	mIsInitialized = true;
}

SpotLightPositionRenderer::~SpotLightPositionRenderer()
{

}

void SpotLightPositionRenderer::InitializeUniforms(Shader & shader)
{
	const char * uniformNames[__uniforms_count__] =
	{
		"u_LightDescSampler",
		"u_LightDataSampler",
		"u_LightOffset",
	};

	shader.Use();
	shader.AddUniforms(uniformNames, __uniforms_count__);

	glUniform1i(shader.GetUniform(u_LightDescSampler), 0);
	glUniform1i(shader.GetUniform(u_LightDataSampler), 1);

	shader.SetupFrameDataBlockBinding();
	shader.UnUse();

	GL_CHECK_ERRORS;
}

void SpotLightPositionRenderer::InternalRender(Shader & shader, GLsizei instanceCount, GLint lightOffset, GLuint lightDescBufferId, GLuint lightDataBufferId)
{
	glDisable(GL_CULL_FACE);
	shader.Use();
	glBindVertexArray(mVaoID);

	glUniform1i(shader.GetUniform(u_LightOffset), lightOffset);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, lightDescBufferId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, lightDataBufferId);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, mIndexCount, instanceCount);

	glBindVertexArray(0);
	shader.UnUse();
	glEnable(GL_CULL_FACE);
}

void SpotLightPositionRenderer::Render()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Spot_Light];
	InternalRender(mShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Spot_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}

void SpotLightPositionRenderer::RenderWireFrame()
{
	Engine * engine = Engine::GetInstance();
	auto lights = engine->mLights[Lights::Light::Spot_Light];
	InternalRender(mWireFrameShader, (GLsizei)lights->GetCount(), engine->mLightDescIndexOffsets[Lights::Light::Spot_Light], engine->mLightDescBuffer.GetTextureId(), engine->mLightDataBuffer.GetTextureId());
}




	} // namespace Renderers
} // namespace CoreFx