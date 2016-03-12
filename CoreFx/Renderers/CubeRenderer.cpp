#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{


CubeRenderer::CubeRenderer(std::string const & texture, std::uint8_t materialCount, size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Cube, 2>(materialCount * Property_Per_Material, capacity, pageSize)
	, mTexture(nullptr)
	, mMaterialCount(materialCount)
	, mIsMaterialIndexBufferSet(false)
	, mIsMaterialDataBufferSet(false)
{
	memset(mMaterials.GetData(), 0, mMaterials.GetDataSize());

	std::cout << std::endl;
	std::cout << "Initialize CubeRenderer...." << std::endl;

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/cube.vs.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/cube.gs.glsl");
#ifdef FORWARD_RENDERING
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube.forward.fs.glsl");
#else
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube.deferred.fs.glsl");
#endif //FORWARD_RENDERING

	mShader.CreateAndLinkProgram();

	mShader.Use();
		mShader.AddAttribute("in_Position");
		mShader.AddAttribute("in_Normal");
		mShader.AddAttribute("in_TexUV");

		mShader.AddUniform("perInstanceDataSampler");
		mShader.AddUniform("materialIndexSampler");
#ifdef FORWARD_RENDERING
		mShader.AddUniform("materialDataSampler");
		mShader.AddUniform("lightDescSampler");
		mShader.AddUniform("lightDataSampler");
		mShader.AddUniform("textureSampler");
#else
		mShader.AddUniform("u_MaterialBaseIndex");
#endif // FORWARD_RENDERING
		
		//pass values of constant uniforms at initialization
		glUniform1i(mShader.GetUniform("perInstanceDataSampler"), 0);
		glUniform1i(mShader.GetUniform("materialIndexSampler"), 1);

#ifdef FORWARD_RENDERING
		glUniform1i(mShader.GetUniform("materialDataSampler"), 2);
		glUniform1i(mShader.GetUniform("textureSampler"), 3);
		glUniform1i(mShader.GetUniform("lightDescSampler"), 4);
		glUniform1i(mShader.GetUniform("lightDataSampler"), 5);
#else
#endif // FORWARD_RENDERING

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;


	std::cout << std::endl;
	std::cout << "Vertex attribute index : " << std::endl;
	std::cout << "\t in_Position : " << mShader.GetAttribute("in_Position") << std::endl;
	std::cout << "\t in_Normal : " << mShader.GetAttribute("in_Normal") << std::endl;
	std::cout << "\t in_TexUV : " << mShader.GetAttribute("in_TexUV") << std::endl;
	std::cout << std::endl;
	std::cout << "Uniform attribute index : " << std::endl;
	std::cout << "\t perInstanceDataSampler : " << mShader.GetUniform("perInstanceDataSampler") << std::endl;

#ifdef FORWARD_RENDERING
	std::cout << "\t materialIndexSampler : " << mShader.GetUniform("materialIndexSampler") << std::endl;
	std::cout << "\t materialDataSampler : " << mShader.GetUniform("materialDataSampler") << std::endl;
	std::cout << "\t lightDescSampler : " << mShader.GetUniform("lightDescSampler") << std::endl;
	std::cout << "\t textureSampler : " << mShader.GetUniform("textureSampler") << std::endl;
#else
#endif // FORWARD_RENDERING

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

	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (GetCapacity() * sizeof(PerInstanceData)), nullptr);

#ifdef FORWARD_RENDERING	
	mMaterialDataBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, mMaterials.GetDataSize(), nullptr);
#else
#endif // FORWARD_RENDERING

	mMaterialIndexBuffer.CreateResource(GL_STATIC_DRAW, GL_R8UI, GetCapacity() * sizeof(std::uint8_t), nullptr);

	std::cout << "\t mVaoID : " << mVaoID << std::endl;
	std::cout << "\t mVboID[VBO_Vertex] : " << mVboIDs[VBO_Vertex] << std::endl;
	std::cout << "\t mVboID[VBO_Index] : " << mVboIDs[VBO_Index] << std::endl;

	std::cout << "... CubeRenderer initialized!" << std::endl << std::endl;
}


CubeRenderer::~CubeRenderer()
{
	mModelMatrixBuffer.ReleaseResource();

#ifdef FORWARD_RENDERING	
	mMaterialDataBuffer.ReleaseResource();
#else
#endif // FORWARD_RENDERING

	mMaterialIndexBuffer.ReleaseResource();

	Engine::GetInstance()->GetTextureManager()->ReleaseTexture2D(mTexture);
}

void CubeRenderer::Render()
{
#ifdef FORWARD_RENDERING
	Engine * engine = Engine::GetInstance();

	if (!mIsMaterialDataBufferSet)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, mMaterialDataBuffer.GetBufferId());

		GLbitfield * matDataBuffer = (GLbitfield *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		memcpy(matDataBuffer, mMaterials.GetData(), mMaterials.GetDataSize());
		glUnmapBuffer(GL_TEXTURE_BUFFER);

		mIsMaterialDataBufferSet = true;
	}
#else
#endif // FORWARD_RENDERING

	if (!mIsMaterialIndexBufferSet)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetBufferId());

		std::uint8_t * matIndexBuffer = (std::uint8_t *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		assert(matIndexBuffer != nullptr);
		if (matIndexBuffer != nullptr)
		{
			mObjs.ForEach([this, matIndexBuffer](Renderables::Cube* obj)
			{
				matIndexBuffer[obj->GetInstanceId()] = obj->GetMaterialIndex();
			});

			glUnmapBuffer(GL_TEXTURE_BUFFER);
		}
		mIsMaterialIndexBufferSet = true;
	}

	glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetBufferId());
	GLintptr offset = 0;
	mObjs.ForEach([this, &offset](Renderables::Cube* obj)
	{
		if (obj->GetFrame()->IsModified())
		{
			std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, sizeof(PerInstanceData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
			GL_CHECK_ERRORS;
			//memcpy(buffer, glm::value_ptr(obj->GetFrame()->GetMatrix()), sizeof(glm::mat4));
			memcpy(buffer, glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetRealPart()), sizeof(glm::quat));
			memcpy(buffer + sizeof(glm::quat), glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetDualPart()), sizeof(glm::quat));
			glUnmapBuffer(GL_TEXTURE_BUFFER);
		}
		offset += sizeof(PerInstanceData);
	});

	mShader.Use();
		glBindVertexArray(mVaoID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetTextureId());

#ifdef FORWARD_RENDERING
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_BUFFER, mMaterialDataBuffer.GetTextureId());

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mTexture->GetId());

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_BUFFER, engine->GetLightDescBuffer().GetTextureId());

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_BUFFER, engine->GetLightDataBuffer().GetTextureId());
#else
#endif // FORWARD_RENDERING

			glUniform1i(mShader.GetUniform("u_MaterialBaseIndex"), GetMaterialBaseIndex());

			glDrawElementsInstanced(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
		glBindVertexArray(0);
	mShader.UnUse();

#ifdef FORWARD_RENDERING
	if (engine->IsDrawVertexNormalEnabled())
	{
		const DrawNormalShader & drawVertexNormalShader = engine->GetDrawVertexNormal();
		drawVertexNormalShader.Use();

		drawVertexNormalShader.SetUniformValues();

			glBindVertexArray(mVaoID);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_BUFFER, engine->GetLightDescBuffer().GetTextureId());

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_BUFFER, engine->GetLightDataBuffer().GetTextureId());

				glDrawElementsInstanced(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
			glBindVertexArray(0);
		drawVertexNormalShader.UnUse();
	}
#else
#endif // FORWARD_RENDERING

}


Renderables::Cube * CubeRenderer::CreateCube(std::uint8_t materialIndex)
{
	if (GetCount() < GetCapacity())
	{
		Renderables::Cube *cube = new Renderables::Cube(materialIndex);
		mObjs.Attach(cube);
		mIsMaterialIndexBufferSet = false;
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
	mIsMaterialIndexBufferSet = false;
}

void CubeRenderer::SetMaterial(std::uint8_t materialIndex, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, std::int8_t ambientTextureIndex, std::int8_t diffuseTextureIndex, std::int8_t specularTextureIndex, std::int8_t normalTextureIndex)
{
	assert(materialIndex < mMaterialCount);

	if (materialIndex < mMaterialCount)
	{
		mIsMaterialIndexBufferSet = false;
		mIsMaterialDataBufferSet = false;

		GLuint propertyIndex = Property_Per_Material * materialIndex;

		GLfloat * prop1 = mMaterials.GetProperty(propertyIndex);
		memcpy(prop1, glm::value_ptr(ambient), sizeof(GLfloat) * 3);

		GLfloat * prop2 = mMaterials.GetProperty(propertyIndex + 1);
		memcpy(prop2, glm::value_ptr(diffuse), sizeof(GLfloat) * 3);

		GLfloat * prop3 = mMaterials.GetProperty(propertyIndex + 2);
		memcpy(prop3, glm::value_ptr(specular), sizeof(GLfloat) * 3);
		memcpy(&prop3[3], &shininess, sizeof(GLfloat));

		GLbitfield textureIndexes =
			(diffuseTextureIndex >= 0 ? (diffuseTextureIndex << 24) : 0xFF000000) |
			(specularTextureIndex >= 0 ? (specularTextureIndex << 16) : 0x00FF0000) |
			(normalTextureIndex >= 0 ? (normalTextureIndex << 8) : 0x0000FF00) |
			(ambientTextureIndex >= 0 ? ambientTextureIndex : 0x000000FF);
		memcpy(&prop1[3], &textureIndexes, sizeof(GLfloat));
	}
}


	} // namespace Renderers
} // namespace CoreFx
