#pragma once

#ifndef COREFX_RENDERABLES_MODEL_H
#define COREFX_RENDERABLES_MODEL_H


namespace CoreFx
{
	namespace Renderers
	{
		class ModelRenderer;
	}
	namespace Renderables
	{


class Model : public SceneObject
{
	template<typename T_Object, int T_vbo_count, typename T_MainShader = Shader, typename T_WireFrameShader = Shader> friend class Renderers::SceneObjectRenderer;
	friend class Renderers::ModelRenderer;

protected:

	Model(GLuint modelIndex);
	virtual ~Model();


	GLuint GetModelIndex() const
	{
		return mModelIndex;
	}

	GLuint GetMatrixIndex() const
	{
		return mMatrixIndex;
	}

	void SetMatrixIndex(GLuint value)
	{
		mMatrixIndex = value;
	}

private:

	GLuint mModelIndex;
	GLuint mMatrixIndex;
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_MODEL_H