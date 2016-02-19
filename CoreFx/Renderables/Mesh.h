#pragma once

#ifndef COREFX_RENDERABLES_MESH_H
#define COREFX_RENDERABLES_MESH_H


namespace CoreFx
{
	namespace Renderables
	{


class Mesh : public RenderableObject<1>
{
public:
	Mesh(const Geometry::MeshData& meshData);
	virtual ~Mesh();

private:

	std::vector<Geometry::SubMeshDesc> mSubMeshDescList;
	std::vector<Geometry::MaterialData> mMaterialList;

};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_MESH_H