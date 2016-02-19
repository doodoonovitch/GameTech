#pragma once
#ifndef COREFX_RENDERER_H
#define COREFX_RENDERER_H


namespace CoreFx
{

		class Renderer;
		typedef InstanceContainer<Renderer*> RendererContainer;


class Renderer : public ObjectInstance
{
public:

	Renderer();
	virtual ~Renderer();

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void Render() = 0;

};




} // namespace CoreFx
#endif // COREFX_RENDERER_H