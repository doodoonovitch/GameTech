#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{




Model::Model(GLuint modelIndex)
	: SceneObject(true)
	, mModelIndex(modelIndex)
	, mMatrixIndex(0)
{
}


Model::~Model()
{
}

 


	} // namespace Renderables
} // namespace CoreFx