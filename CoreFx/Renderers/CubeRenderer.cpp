#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{


CubeRenderer::CubeRenderer(std::string const & texture, size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Cube, 3>(capacity, pageSize)
	, mTexture(nullptr)
{
	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/cube_shader.vert");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/cube_shader.geom");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube_shader.frag");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddAttribute("vPos");
		mShader.AddAttribute("vTexUV");
		mShader.AddAttribute("mPerInstanceModel");

		mShader.AddUniform("texSampler1");

		//pass values of constant uniforms at initialization
		glUniform1i(mShader("texSampler1"), 0);
	mShader.UnUse();

	GL_CHECK_ERRORS;

	mShader.SetupFrameDataBlockBinding();


	std::cout << std::endl;
	std::cout << "Cube info..." << std::endl;
	std::cout << "Vertex attribute index : " << std::endl;
	std::cout << "\tvPos : " << mShader["vPos"] << std::endl;
	std::cout << "\tvTexUV : " << mShader["vTexUV"] << std::endl;
	std::cout << "\tmPerInstanceModel : " << mShader["mPerInstanceModel"] << std::endl;
	std::cout << std::endl;
	std::cout << "Uniform attribute index : " << std::endl;
	std::cout << "\ttexSampler1 : " << mShader("texSampler1") << std::endl;
	std::cout << std::endl;

	const float k = 1.f / 1.5f;

	Vertex vertices[24] =
	{
		// Vertex data for face 0
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(0.0f, 0.0f * k) },	// v0
		{ glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec2(0.33f, 0.0f * k) },	// v1
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(0.0f, 0.5f * k) },	// v2
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.33f, 0.5f * k) },	// v3

																			// Vertex data for face 1
		{ glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec2(0.0f, 0.5f * k) },	// v4
		{ glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec2(0.33f, 0.5f * k) },	// v5
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.0f, 1.0f * k) },	// v6
		{ glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec2(0.33f, 1.0f * k) },	// v7

																				// Vertex data for face 2
		{ glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec2(0.66f, 0.5f * k) },	// v8
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(1.0f, 0.5f * k) },	// v9
		{ glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec2(0.66f, 1.0f * k) },	// v10
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(1.0f, 1.0f * k) },	// v11

																			// Vertex data for face 3
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(0.66f, 0.0f * k) },	// v12
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(1.0f, 0.0f * k) },	// v13
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(0.66f, 0.5f * k) },	// v14
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(1.0f, 0.5f * k) },	// v15

																			// Vertex data for face 4
		{ glm::vec3(-1.0f, -1.0f, -1.0f),	glm::vec2(0.33f, 0.0f * k) },	// v16
		{ glm::vec3(1.0f, -1.0f, -1.0f),		glm::vec2(0.66f, 0.0f * k) },	// v17
		{ glm::vec3(-1.0f, -1.0f,  1.0f),	glm::vec2(0.33f, 0.5f * k) },	// v18
		{ glm::vec3(1.0f, -1.0f,  1.0f),		glm::vec2(0.66f, 0.5f * k) },	// v19

																				// Vertex data for face 5
		{ glm::vec3(-1.0f,  1.0f,  1.0f),	glm::vec2(0.33f, 0.5f * k) },	// v20
		{ glm::vec3(1.0f,  1.0f,  1.0f),	glm::vec2(0.66f, 0.5f * k) },	// v21
		{ glm::vec3(-1.0f,  1.0f, -1.0f),	glm::vec2(0.33f, 1.0f * k) },	// v22
		{ glm::vec3(1.0f,  1.0f, -1.0f),		glm::vec2(0.66f, 1.0f * k) }	// v23
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
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(Vertex), &vertices[0].pos.x, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
	glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	GL_CHECK_ERRORS;

	glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
	glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(offsetof(Vertex, uv)));
	GL_CHECK_ERRORS;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 34, &indices[0], GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VBO_PerInstanceData]);
	GLuint mvpLoc = mShader["mPerInstanceModel"];
	// Loop over each column of the matrix...
	for (int i = 0; i < 4; i++)
	{
		// Set up the vertex attribute
		glVertexAttribPointer(mvpLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),	(void *)(sizeof(glm::vec4) * i));
		glEnableVertexAttribArray(mvpLoc + i);
		// Make it instanced
		glVertexAttribDivisor(mvpLoc + i, 1);
	}
	GL_CHECK_ERRORS;

	glBufferData(GL_ARRAY_BUFFER, GetCapacity() * sizeof(PerInstanceData), nullptr, GL_STATIC_DRAW);
	GL_CHECK_ERRORS;

	glBindVertexArray(0);

	mTexture = Engine::GetInstance()->GetTextureManager()->LoadTexture2D(texture);
}


CubeRenderer::~CubeRenderer()
{
	Engine::GetInstance()->GetTextureManager()->ReleaseTexture2D(mTexture);
}

void CubeRenderer::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VBO_PerInstanceData]);

	GLintptr offset = 0;
	mObjs.ForEach([this, &offset](Renderables::Cube* obj)
	{
		if (obj->GetFrame()->HasNewValue())
		{
			PerInstanceData* buffer = (PerInstanceData*)glMapBufferRange(GL_ARRAY_BUFFER, offset, sizeof(PerInstanceData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
			memcpy(glm::value_ptr(buffer->mModel), glm::value_ptr(obj->GetFrame()->GetMatrix()), sizeof(glm::mat4));
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		offset += sizeof(PerInstanceData);
	});

	mShader.Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexture->GetId());

	glBindVertexArray(mVaoID);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
	//glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
	mShader.UnUse();
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

void CubeRenderer::DeleteCube(Renderables::Cube * cube)
{
	if (cube == nullptr)
		return;
	mObjs.Detach(cube);
	delete cube;
}



	} // namespace Renderers
} // namespace CoreFx
