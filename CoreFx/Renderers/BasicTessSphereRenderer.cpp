#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



BasicTessSphereRendererBase::BasicTessSphereRendererBase(GLuint maxSphereCount)
	: RendererHelper<1, BasicTessSphereShader, Shader>(0, "BasicTessSphereRenderer", "BasicTessSphereWireFrameRenderer", Renderer::Forward_Pass)
	, mVertexArrayCapacity(maxSphereCount)
{
}

BasicTessSphereRendererBase::~BasicTessSphereRendererBase()
{
}

void BasicTessSphereRendererBase::Initialize()
{
	mShader.LoadShaders();

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);

	glBindVertexArray(mVaoID);

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, mVertexArrayCapacity * sizeof(GLfloat) * 4, nullptr, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;
}

void BasicTessSphereRendererBase::UpdateVertexArrayBuffer(std::uint8_t * buffer, GLuint vertexCount, GLuint vertexArrayCapacity)
{
	if (vertexArrayCapacity > mVertexArrayCapacity)
	{
		mVertexArrayCapacity = vertexArrayCapacity;

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat) * 4, buffer, GL_STATIC_DRAW);
	}
	else
	{
		std::uint8_t * dataBuffer = (std::uint8_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (dataBuffer != nullptr)
		{
			memcpy(dataBuffer, buffer, sizeof(GLfloat) * 4 * vertexCount);
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		else
		{
			PRINT_ERROR("BasicTessSphereRender has failed to map vertex array buffer.");
		}
	}
}


BasicTessSphereRenderer::BasicTessSphereRenderer(GLuint maxSphereCount)
	: BasicTessSphereRendererBase(maxSphereCount)
	, mIsModified(false)
{
	mSphereList.reserve(mVertexArrayCapacity);

	std::cout << std::endl;
	std::cout << "Initialize BasicTessSphereRenderer...." << std::endl;

	Initialize();

	std::cout << "... BasicTessSphereRenderer initialized!" << std::endl << std::endl;
}

BasicTessSphereRenderer::~BasicTessSphereRenderer()
{
}

void BasicTessSphereRenderer::UpdateVertexArrayBuffer()
{
	BasicTessSphereRendererBase::UpdateVertexArrayBuffer((std::uint8_t *)mSphereList.data(), (GLuint)mSphereList.size(), (GLuint)mSphereList.capacity());
}

void BasicTessSphereRenderer::Render()
{
	mShader.Use();
	glBindVertexArray(mVaoID);

	if (mIsModified)
	{
		UpdateVertexArrayBuffer();
		mIsModified = false;
	}

	glDrawArrays(GL_POINT, 0, (GLuint)mSphereList.size());
	glBindVertexArray(0);
	mShader.UnUse();
}

void BasicTessSphereRenderer::RenderWireFrame()
{
	Render();
}




	} // namespace Renderers
} // namespace CoreFx