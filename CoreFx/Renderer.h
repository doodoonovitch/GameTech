#pragma once
#ifndef COREFX_RENDERER_H
#define COREFX_RENDERER_H


namespace CoreFx
{

	class Engine;
	class Renderer;
	typedef InstanceContainer<Renderer*> RendererContainer;


class Renderer : public ObjectInstance
{
	friend class Engine;
public:

	Renderer(GLuint propertyCount);
	virtual ~Renderer();

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Render() = 0;

	const PropertyData & GetMaterials() const {	return mMaterials; }
	GLint GetMaterialBaseIndex() const { return mMaterialBaseIndex; }

protected:

	PropertyData mMaterials;
	GLint mMaterialBaseIndex;
};




} // namespace CoreFx
#endif // COREFX_RENDERER_H