#pragma once

#ifndef COREFX_RENDERABLES_MESH_H
#define COREFX_RENDERABLES_MESH_H


namespace CoreFx
{
	namespace Renderers
	{
		class MeshRenderer;
	}
	namespace Renderables
	{


		class Mesh : public SceneObject
		{

			template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
			friend class Renderers::MeshRenderer;

		protected:

			Mesh();
			virtual ~Mesh();

		private:

		};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_MESH_H