#pragma once

#ifndef COREFX_RENDERERS_MESHRENDERER_H
#define COREFX_RENDERERS_MESHRENDERER_H


namespace CoreFx
{
	namespace Renderers
	{




class MeshRenderer : public SceneObjectRenderer<Renderables::Mesh, 1>
{
public:
	MeshRenderer(const Geometry::MeshData& meshData, size_t capacity = 64, size_t pageSize = 10);
	virtual ~MeshRenderer();

	virtual void Render() override;


private:

	std::vector<Geometry::SubMeshDesc> mSubMeshDescList;
	std::vector<Geometry::MaterialData> mMaterialList;

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_MESHRENDERER_H
