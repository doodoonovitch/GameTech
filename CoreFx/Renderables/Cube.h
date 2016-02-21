#pragma once

#ifndef COREFX_RENDERABLES_CUBE_H
#define COREFX_RENDERABLES_CUBE_H


namespace CoreFx
{
	namespace Renderers
	{
		class CubeRenderer;
	}

	namespace Renderables
	{



class Cube : public SceneObject
{
	template<typename T_Object, int T_vbo_count> friend class Renderers::SceneObjectRenderer;
	friend class Renderers::CubeRenderer;

protected:

	Cube(GLuint textureIndex);
	virtual ~Cube();

private:

	GLuint mTextureIndex;

};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_CUBE_H