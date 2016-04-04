#include "stdafx.h"
#include "CoreFx.h"

namespace CoreFx
{
	namespace Renderers
	{


CubeRenderer::CubeRenderer(std::uint16_t materialCount, size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Cube, 2>(materialCount * Property_Per_Material, capacity, pageSize, "CubeRenderer")
	, mMaterialCount(materialCount)
	, mMaterialTextureIndexesList(materialCount)
	, mIsMaterialIndexBufferSet(false)
	, mIsMaterialDataBufferSet(false)
{
	memset(mMaterials.GetData(), 0, mMaterials.GetDataSize());

	const float k = 1.f / 1.5f;
	
	GLfloat vertices[] =
	{
		// Vertex data for face 0
		-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f * k,		// v0
		1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.0f * k,	// v1
		-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.5f * k,		// v2
		1.0f,  1.0f,  1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.5f * k,	// v3

		// Vertex data for face 1
		1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.5f * k,		// v4
		1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.5f * k,	// v5
		1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f * k,		// v6
		1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 1.0f * k,	// v7

		// Vertex data for face 2
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.5f * k,	// v8
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.5f * k,		// v9
		1.0f,  1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.66f, 1.0f * k,	// v10
		-1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f * k,		// v11

		// Vertex data for face 3
		-1.0f, -1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.0f * k,	// v12
		-1.0f, -1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f * k,		// v13
		-1.0f,  1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.5f * k,	// v14
		-1.0f,  1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.5f * k,		// v15

		// Vertex data for face 4
		-1.0f, -1.0f, -1.0f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.0f * k,	// v16
		1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.0f * k,	// v17
		-1.0f, -1.0f,  1.0f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.5f * k,	// v18
		1.0f, -1.0f,  1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.5f * k,	// v19

		// Vertex data for face 5
		-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 0.5f * k,	// v20
		1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 0.5f * k,	// v21
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.33f, 1.0f * k,	// v22
		1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f,		0.66f, 1.0f * k		// v23
	};

	//GLushort indices[34] =
	//{
	//	0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
	//	4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
	//	8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
	//	12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
	//	16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
	//	20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
	//};
	
	GLushort indices[36] =
	{
		 0,  1,  2,  3,  2,  1,		// Face 0 - triangle strip ( v0,  v1,  v2,  v3)
		 4,  5,  6,  7,  6,	 5,		// Face 1 - triangle strip ( v4,  v5,  v6,  v7)
		 8,  9, 10, 11, 10,	 9,		// Face 2 - triangle strip ( v8,  v9, v10, v11)
		12, 13, 14, 15, 14,	13,		// Face 3 - triangle strip (v12, v13, v14, v15)
		16, 17, 18, 19, 18,	17,		// Face 4 - triangle strip (v16, v17, v18, v19)
		20, 21, 22, 23, 22, 21		// Face 5 - triangle strip (v20, v21, v22, v23)
	};

	ComputeTangent(vertices, 24, indices, mIndexCount);

	//setup vao and vbo stuff
	CreateBuffers();

	const int vextexStride = 11;
	const int normalOffset = 3;
	const int tangentOffset = 6;
	const int textureOffset = 9;

	PRINT_GEN_VERTEXARRAY("[CubeRenderer]", mVaoID);
	PRINT_GEN_BUFFER("[CubeRenderer]", mVboIDs[VBO_Vertex]);
	PRINT_GEN_BUFFER("[CubeRenderer]", mVboIDs[VBO_Index]);

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[VBO_Vertex]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vextexStride * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE);
		glVertexAttribPointer(Shader::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vextexStride * sizeof(GLfloat), (const GLvoid*)(normalOffset * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::TANGENT_ATTRIBUTE);
		glVertexAttribPointer(Shader::TANGENT_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vextexStride * sizeof(GLfloat), (const GLvoid*)(tangentOffset * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
		glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, vextexStride * sizeof(GLfloat), (const GLvoid*)(textureOffset * sizeof(GLfloat)));
		GL_CHECK_ERRORS;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndexCount, &indices[0], GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
 
	glBindVertexArray(0);
		
	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (GetCapacity() * sizeof(PerInstanceData)), nullptr);

	mMaterialIndexBuffer.CreateResource(GL_STATIC_DRAW, GL_R8UI, GetCapacity() * sizeof(std::uint8_t), nullptr);

	PRINT_GEN_TEXTUREBUFFER("[CubeRenderer]", mModelMatrixBuffer);
	PRINT_GEN_TEXTUREBUFFER("[CubeRenderer]", mMaterialIndexBuffer);
}


CubeRenderer::~CubeRenderer()
{
	mModelMatrixBuffer.ReleaseResource();

	mMaterialIndexBuffer.ReleaseResource();
}

void CubeRenderer::ComputeTangent(GLfloat vertices[], int vertexCount, GLushort indices[], int indexCount)
{
	const int Vertex_X = 0;
	const int Vertex_Y = 1;
	const int Vertex_Z = 2;

	const int Normal_X = 3;
	const int Normal_Y = 4;
	const int Normal_Z = 5;

	const int Tangent_X = 6;
	const int Tangent_Y = 7;
	const int Tangent_Z = 8;

	const int Texture_U = 9;
	const int Texture_V = 10;

	const int VertexStride = 11;

	for (int index = 0; index < indexCount; )
	{
		GLfloat * v0 = &vertices[indices[index] * VertexStride]; ++index;
		GLfloat * v1 = &vertices[indices[index] * VertexStride]; ++index;
		GLfloat * v2 = &vertices[indices[index] * VertexStride]; ++index;

		glm::vec3 edge1;
		glm::vec3 edge2;

		{
			glm::vec3 vertex0(v0[Vertex_X], v0[Vertex_Y], v0[Vertex_Z]);
			glm::vec3 vertex1(v1[Vertex_X], v1[Vertex_Y], v1[Vertex_Z]);
			glm::vec3 vertex2(v2[Vertex_X], v2[Vertex_Y], v2[Vertex_Z]);

			edge1 = vertex1 - vertex0;
			edge2 = vertex2 - vertex0;
		}

		float deltaU1 = v1[Texture_U] - v0[Texture_U];
		float deltaV1 = v1[Texture_V] - v0[Texture_V];
		float deltaU2 = v2[Texture_U] - v0[Texture_U];
		float deltaV2 = v2[Texture_V] - v0[Texture_V];

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		glm::vec3 tangent;

		tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

		for (int i = 0; i < 3; i++)
		{
			v0[Tangent_X + i] += tangent[i];
			v1[Tangent_X + i] += tangent[i];
			v2[Tangent_X + i] += tangent[i];
		}
	}

	for (int index = 0; index < vertexCount; ++index)
	{
		GLfloat * v0 = &vertices[index * VertexStride];
		glm::vec3 v(v0[Tangent_X], v0[Tangent_Y], v0[Tangent_Z]);
		float length = glm::length(v);
		v0[Tangent_X] /= length;
		v0[Tangent_Y] /= length;
		v0[Tangent_Z] /= length;
	}
}

void CubeRenderer::InitializeShader()
{
	std::cout << std::endl;
	std::cout << "Initialize CubeRenderer shader...." << std::endl;

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/cube.vs.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/cube.gs.glsl");
	//mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cube.deferred.fs.glsl");

	std::cout << "Loading shader file : shaders/cube.deferred.fs.glsl" << std::endl;
	// fragment shader
	std::vector<std::string> lightFsGlsl(2);
	Shader::MergeFile(lightFsGlsl[0], "shaders/cube.deferred.fs.glsl");
	std::string & textureFuncSource = lightFsGlsl[1];
	Shader::GenerateTexGetFunction(textureFuncSource, (int)mTextureMapping.mMapping.size(), "u_normalMapSampler");
	mShader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	const char * uniformNames[__uniforms_count__] =
	{
		"u_MaterialBaseIndex",
		"u_perInstanceDataSampler",
		"u_materialIndexSampler",
		"u_materialDataSampler"
	};

	mShader.CreateAndLinkProgram();

	mShader.Use();
	mShader.AddUniforms(uniformNames, __uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mShader.GetUniform(u_perInstanceDataSampler), 0);
	glUniform1i(mShader.GetUniform(u_materialIndexSampler), 1);
	glUniform1i(mShader.GetUniform(u_materialDataSampler), 2);


	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_normalMapSampler[%i]", i);
		int uniformIndex = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		if (uniformIndex > 0)
		{
			glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
			std::cout << "\t" << uniformName << " : " << uniformIndex << std::endl;
		}
	}

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	std::cout << "... CubeRenderer shader initialized!" << std::endl << std::endl;

}

void CubeRenderer::Render()
{
	if (!mIsMaterialIndexBufferSet)
	{
		glBindBuffer(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetBufferId());

		std::uint8_t * matIndexBuffer = (std::uint8_t *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY); GL_CHECK_ERRORS;
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
			std::uint8_t * buffer = (std::uint8_t *)glMapBufferRange(GL_TEXTURE_BUFFER, offset, sizeof(PerInstanceData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT); GL_CHECK_ERRORS;

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

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_BUFFER, Engine::GetInstance()->GetMaterialDataBuffer().GetTextureId());
			
			for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
			{
				glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
				glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureMapping.mMapping[i].mTexture->GetResourceId());
			}

			glUniform1i(mShader.GetUniform(u_MaterialBaseIndex), GetMaterialBaseIndex());

			glDrawElementsInstanced(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
		glBindVertexArray(0);
	mShader.UnUse();

}

void CubeRenderer::DebugRender() 
{
	Engine * engine = Engine::GetInstance();

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

		glDrawElementsInstanced(GL_TRIANGLE_STRIP, mIndexCount, GL_UNSIGNED_SHORT, 0, (GLsizei)mObjs.GetCount());
		glBindVertexArray(0);
		drawVertexNormalShader.UnUse();
	}
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

void CubeRenderer::SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, const glm::vec3& specular, int16_t shininess, TextureIndex diffuseTextureIndex, TextureIndex specularTextureIndex, TextureIndex normalTextureIndex)
{
	assert(materialIndex < mMaterialCount);

	if (materialIndex < mMaterialCount)
	{
		mIsMaterialIndexBufferSet = false;

		GLuint propertyIndex = Property_Per_Material * materialIndex;

		GLfloat * prop1 = mMaterials.GetProperty(propertyIndex);
		memcpy(prop1, glm::value_ptr(diffuse), sizeof(GLfloat) * 3);

		GLfloat * prop2 = mMaterials.GetProperty(propertyIndex + 1);
		memcpy(prop2, glm::value_ptr(specular), sizeof(GLfloat) * 3);

		GLbitfield shininessNormalIndexes = ((shininess & 0xFFFF) << 16);
		memcpy(&prop2[3], &shininessNormalIndexes, sizeof(GLfloat));

		MaterialTextureIndexes & texIndexes = mMaterialTextureIndexesList[materialIndex];
		texIndexes.mDiffuse = diffuseTextureIndex;
		texIndexes.mSpecular = specularTextureIndex;
		texIndexes.mNormal = normalTextureIndex;
	}
}

void CubeRenderer::SetMaterial(std::uint16_t materialIndex, const MaterialDesc & mat)
{
	SetMaterial(materialIndex, mat.mDiffuse, mat.mSpecular, mat.mShininess, mat.mDiffuseTextureIndex, mat.mSpecularTextureIndex, mat.mNormalTextureIndex);
}


void CubeRenderer::UpdateMaterialTextureIndex()
{
	const TextureInfoList & texInfo = GetTextureInfoList();

	for (GLuint materialIndex = 0; materialIndex < (GLuint)mMaterialTextureIndexesList.size(); ++materialIndex)
	{
		MaterialTextureIndexes & texIndexes = mMaterialTextureIndexesList[materialIndex];

		std::uint8_t diffuseTextureIndex = texIndexes.mDiffuse != NoTexture ? (std::uint8_t)texInfo[texIndexes.mDiffuse].GetLayerIndex() : 0xFF;
		std::uint8_t specularTextureIndex = texIndexes.mSpecular != NoTexture ? (std::uint8_t)texInfo[texIndexes.mSpecular].GetLayerIndex() : 0xFF;
		std::uint8_t normalTextureIndex = texIndexes.mNormal != NoTexture ? (std::uint8_t)texInfo[texIndexes.mNormal].GetLayerIndex() : 0xFF;

		std::uint8_t diffuseSamplerIndex = texIndexes.mDiffuse != NoTexture ? (std::uint8_t)texInfo[texIndexes.mDiffuse].GetSamplerIndex() : 0xFF;
		std::uint8_t specularSamplerIndex = texIndexes.mSpecular != NoTexture ? (std::uint8_t)texInfo[texIndexes.mSpecular].GetSamplerIndex() : 0xFF;
		std::uint8_t normalSamplerIndex = texIndexes.mNormal != NoTexture ? (std::uint8_t)texInfo[texIndexes.mNormal].GetSamplerIndex() : 0xFF;

		GLuint propertyIndex = Property_Per_Material * materialIndex;

		GLfloat * prop1 = mMaterials.GetProperty(propertyIndex);
		GLbitfield diffuseSpecularIndexes = ((diffuseSamplerIndex & 0xFF) << 24) | ((diffuseTextureIndex & 0xFF) << 16) | ((specularSamplerIndex & 0xFF) << 8) | (specularTextureIndex & 0xFF);
		memcpy(&prop1[3], &diffuseSpecularIndexes, sizeof(GLfloat));
		
		GLfloat * prop2 = mMaterials.GetProperty(propertyIndex + 1);
		GLbitfield shininessNormalIndexes;
		memcpy(&shininessNormalIndexes, &prop2[3], sizeof(GLfloat));
		shininessNormalIndexes = (shininessNormalIndexes & 0xFFFF0000) | ((normalSamplerIndex & 0xFF) << 8) | (normalTextureIndex & 0xFF);
		memcpy(&prop2[3], &shininessNormalIndexes, sizeof(GLfloat));
	}

	InitializeShader();
}


	} // namespace Renderers
} // namespace CoreFx
