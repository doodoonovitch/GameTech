#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{




Model::Model(std::uint8_t materialGroupIndex)
	: SceneObject(true)
	, mMaterialGroupIndex(materialGroupIndex)
{
}


Model::~Model()
{
}

 


	} // namespace Renderables
} // namespace CoreFx