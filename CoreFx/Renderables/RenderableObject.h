#pragma once

#ifndef COREFX_RENDERABLEOBJECT_H
#define COREFX_RENDERABLEOBJECT_H


namespace CoreFx
{
	namespace Renderables
	{



class RenderableObject
{
public:
	RenderableObject(void);
	virtual ~RenderableObject(void);
	virtual void Render(const float* MVP)=0;

protected:
	GLuint _vaoID;
	GLuint _vboVerticesID;
	GLuint _vboIndicesID;
	int _totalIndices; 
	Shader _shader;
};




	} // namespace Renderables
} // namespace CoreFx

#endif // COREFX_RENDERABLEOBJECT_H