#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{


CubeRenderer::CubeRenderer(std::string const & texture, size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Cube, 2>(capacity, pageSize)
	, mTexture(nullptr)
	, mModelMatrixBuffer(0)
{
	std::cout << std::endl;
	std::cout << "Initialize CubeRenderer...." << std::endl;

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/cube_shader.vert");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/cube_shader.geom");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube_shader.frag");

	mShader.CreateAndLinkProgram();

	mShader.Use();
		mShader.AddAttribute("in_Position");
		mShader.AddAttribute("in_Normal");
		mShader.AddAttribute("in_TexUV");

		mShader.AddUniform("texSampler1");
		mShader.AddUniform("perInstanceDataSampler");
		
		//pass values of constant uniforms at initialization
		glUniform1i(mShader("texSampler1"), 0);
		glUniform1i(mShader("perInstanceDataSampler"), 1);

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;
	std::cout << "Vertex attribute index : " << std::endl;
	std::cout << "\t in_Position : " << mShader["in_Position"] << std::endl;
	std::cout << "\t in_Normal : " << mShader["in_Normal"] << std::endl;
	std::cout << "\t in_TexUV : " << mShader["in_TexUV"] << std::endl;
	std::cout << std::endl;
	std::cout << "Uniform attribute index : " << std::endl;
	std::cout << "\t texSampler1 : " << mShader("texSampler1") << std::endl;
	std::cout << "\t perInstanceDataSampler : " << mShader("perInstanceDataSampler") << std::endl;
	std::cout << std::endl;

	const float k = 1.f / 1.5f;
	
	GLfloat vertices[] =
	{
		// Vertex data for face 0
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f * k,		// v0
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.33f, 0.0f * k,	// v1
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.5f * k,		// v2
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.33f, 0.5f * k,	// v3

		// Vertex data for face 1
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.5f * k,		// v4
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.33f, 0.5f * k,	// v5
		1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f * k,		// v6
		1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.33f, 1.0f * k,	// v7

		// Vertex data for face 2
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.66f, 0.5f * k,	// v8
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		1.0f, 0.5f * k,		// v9
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.66f, 1.0f * k,	// v10
		-1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f * k,		// v11

		// Vertex data for face 3
		-1.0f, -1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.66f, 0.0f * k,	// v12
		-1.0f, -1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f * k,		// v13
		-1.0f,  1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.66f, 0.5f * k,	// v14
		-1.0f,  1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.5f * k,		// v15

		// Vertex data for face 4
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f, 0.0f,		0.33f, 0.0f * k,	// v16
		1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,		0.66f, 0.0f * k,	// v17
		-1.0f, -1.0f,  1.0f,	0.0f, -1.0f, 0.0f,		0.33f, 0.5f * k,	// v18
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f, 0.0f,		0.66f, 0.5f * k,	// v19

		// Vertex data for face 5
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,		0.33f, 0.5f * k,	// v20
		1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 0.0f,		0.66f, 0.5f * k,	// v21
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,		0.33f, 1.0f * k,	// v22
		1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.66f, 1.0f * k		// v23
	};

	GLushort indices[34] =
	{
		0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
		4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
		8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
		12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
		16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
		20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
	};
	
	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VBO_Vertex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE);
		glVertexAttribPointer(Shader::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
		glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const GLvoid*)(6 * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 34, &indices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	mTexture = Engine::GetInstance()->GetTextureManager()->LoadTexture2D(texture);

	glGenTextures(1, &mModelMatrixBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, mModelMatrixBuffer);
	GL_CHECK_ERRORS;

	glBufferData(GL_TEXTURE_BUFFER, GetCapacity() * sizeof(PerInstanceData), nullptr, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	std::cout << "\t mVaoID : " << mVaoID << std::endl;
	std::cout << "\t mVboID[VBO_Vertex] : " << mVboIDs[VBO_Vertex] << std::endl;
	std::cout << "\t mVboID[VBO_Index] : " << mVboIDs[VBO_Index] << std::endl;

	mDrawVertexNormalShader.LoadShaders();

	std::cout << "... CubeRenderer initialized!" << std::endl << std::endl;
}


CubeRenderer::~CubeRenderer()
{
	glDeleteTextures(1, &mModelMatrixBuffer);
	Engine::GetInstance()->GetTextureManager()->ReleaseTexture2D(mTexture);
}

void CubeRenderer::Render()
{
	glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, mModelMatrixBuffer);

	GLintptr offset = 0;
	mObjs.ForEach([this, &offset](Renderables::Cube* obj)
	{
		if (obj->GetFrame()->HasNewValue())
		{
			void * buffer = glMapBufferRange(GL_TEXTURE_BUFFER, offset, sizeof(PerInstanceData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
			memcpy(buffer, glm::value_ptr(obj->GetFrame()->GetMatrix()), sizeof(glm::mat4));
			glUnmapBuffer(GL_TEXTURE_BUFFER);
		}
		offset += sizeof(PerInstanceData);
	});

	mShader.Use();
		glBindVertexArray(mVaoID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mTexture->GetId());

			glActiveTexture(GL_TEXTURE1);
			glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer);

			glDrawElementsInstanced(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
		glBindVertexArray(0);
	mShader.UnUse();

	if (Engine::GetInstance()->IsDrawVertexNormalEnabled())
	{
		mDrawVertexNormalShader.Use();

			mDrawVertexNormalShader.SetUniformValues();

			glBindVertexArray(mVaoID);

				glActiveTexture(GL_TEXTURE0);
				glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer);

				glDrawElementsInstanced(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
			glBindVertexArray(0);
		mDrawVertexNormalShader.UnUse();
	}
}


Renderables::Cube * CubeRenderer::CreateCube(GLuint textureIndex)
{
	if (GetCount() < GetCapacity())
	{
		Renderables::Cube *cube = new Renderables::Cube(textureIndex);
		mObjs.Attach(cube);
		return cube;
	}
	else
		return nullptr;
}

void CubeRenderer::DeleteCube(Renderables::Cube *& cube)
{
	if (cube == nullptr)
		return;
	mObjs.Detach(cube);
	SAFE_DELETE(cube);
}



	} // namespace Renderers
} // namespace CoreFx
