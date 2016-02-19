#pragma once

#ifndef COREFX_RENDERABLES_CUBE_H
#define COREFX_RENDERABLES_CUBE_H


namespace CoreFx
{
	namespace Renderables
	{


class Cube : public SceneObject
{
public:
	Cube(GLuint textureIndex);
	virtual ~Cube();

private:

	GLuint mTextureIndex;

};



	} // namespace Renderables
} // namespace CoreFx
#endif // COREFX_RENDERABLES_CUBE_H