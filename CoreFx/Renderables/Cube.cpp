#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Cube::Cube(GLuint textureIndex)
	: SceneObject(true)
	, mTextureIndex(textureIndex)
{
}


Cube::~Cube()
{

}
 



	} // namespace Renderables
} // namespace CoreFx