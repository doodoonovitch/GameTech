#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{



Cube::Cube(std::uint8_t materialIndex)
	: SceneObject(true)
	, mMaterialIndex(materialIndex)
{
}


Cube::~Cube()
{

}
 



	} // namespace Renderables
} // namespace CoreFx