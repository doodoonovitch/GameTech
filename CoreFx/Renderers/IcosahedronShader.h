#pragma once

#ifndef COREFX_RENDERERS_BASICICOSAHEDRONSHADER_H
#define COREFX_RENDERERS_BASICICOSAHEDRONSHADER_H


namespace CoreFx
{
	namespace Renderers
	{


class BasicIcosahedronShader : public Shader
{
public:

	enum EUniformIndex
	{
		u_InnerTessLevel,
		u_OuterTessLevel,
		u_DrawColor,

		__uniforms_count__
	};

public:
	BasicIcosahedronShader(const char * shaderTitle = "BasicIcosahedronShader");
	~BasicIcosahedronShader();

	void LoadShaders();

private:

};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_BASICICOSAHEDRONSHADER_H