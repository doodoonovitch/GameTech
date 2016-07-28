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

	Model(std::uint8_t materialGroupIndex);
	virtual ~Model();


	std::uint8_t GetMaterialGroupIndex() const
	{
		return mMaterialGroupIndex;
	}

private:

	std::uint8_t mMaterialGroupIndex;
};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_MODEL_H