#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{




ModelRenderer::ModelRenderer(size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Model, 3>((GLuint)(0 * Property_Per_Material), capacity, pageSize, "ModelRenderer", "ModelWireFrameRenderer")
	, mMaterialCount(0)
	, mDrawCmdCount(0)
	, mMaterialTextureIndexesList(0)
	, mIsModelSet(false)
	, mIsShaderBufferSet(false)
{

}

void ModelRenderer::SetModel(const Renderer::VertexDataVector & vertexList, const Renderer::IndexVector & indexList, const Renderer::MaterialDescList & materialDescList, const Renderer::TextureDescList & textureDescList, const Renderer::DrawElementsIndirectCommandList & meshDrawInstanceList)
{
	if (mIsModelSet)
	{
		PRINT_ERROR("[ModelRenderer] The renderer is already initialized with a model!");
		return;
	}

	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize ModelRenderer....");

	mMaterialCount = (GLuint)materialDescList.size();
	mDrawCmdCount = (GLsizei)meshDrawInstanceList.size();
	mMaterialTextureIndexesList.resize((GLuint)materialDescList.size());
	mMeshDrawInstanceList = meshDrawInstanceList;

	mMaterials.Resize(mMaterialCount * Property_Per_Material);

	AddTextures(textureDescList);
	SetMaterials(materialDescList);
	BuildTextureMapping();

	const GLsizei vertexDataSize = sizeof(Renderer::VertexData);

	//setup vao and vbo stuff
	//setup vao and vbo stuff
	CreateBuffers();

	PRINT_GEN_VERTEXARRAY("[ModelRenderer]", mVaoID);
	PRINT_GEN_BUFFER("[ModelRenderer]", mVboIDs[VBO_Vertex]);
	PRINT_GEN_BUFFER("[ModelRenderer]", mVboIDs[VBO_Index]);
	PRINT_GEN_BUFFER("[ModelRenderer]", mVboIDs[VBO_Indirect]);

	glBindVertexArray(mVaoID);
	
		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[VBO_Vertex]);
		glBufferData(GL_ARRAY_BUFFER, vertexList.size() * vertexDataSize, vertexList.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (GLvoid*)offsetof(Renderer::VertexData, mPosition));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
		glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, vertexDataSize, (GLvoid*)offsetof(Renderer::VertexData, mTexUV));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE);
		glVertexAttribPointer(Shader::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (GLvoid*)offsetof(Renderer::VertexData, mNormal));
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::TANGENT_ATTRIBUTE);
		glVertexAttribPointer(Shader::TANGENT_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (GLvoid*)offsetof(Renderer::VertexData, mTangent));
		GL_CHECK_ERRORS;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(GLuint) * indexList.size()), indexList.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	//	 
	glBindVertexArray(0);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Renderer::DrawElementsIndirectCommand) * mDrawCmdCount, mMeshDrawInstanceList.data(), GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	GL_CHECK_ERRORS;


	mModelMatrixBuffer.CreateResource(GL_STATIC_DRAW, GL_RGBA32F, (GetCapacity() * sizeof(PerInstanceData)), nullptr);

	//mMaterialIndexBuffer.CreateResource(GL_STATIC_DRAW, GL_R32UI, mDrawCmdCount * sizeof(GLuint), nullptr);

	LoadTextures();
	UpdateMaterialTextureIndex();


	PRINT_GEN_TEXTUREBUFFER("[ModelRenderer]", mModelMatrixBuffer);
	//PRINT_GEN_TEXTUREBUFFER("[ModelRenderer]", mMaterialIndexBuffer);

	PRINT_MESSAGE("... ModelRenderer initialized!\n");
	PRINT_END_SECTION;
}


ModelRenderer::~ModelRenderer()
{
	mModelMatrixBuffer.ReleaseResource();
	//mMaterialIndexBuffer.ReleaseResource();
}

 
void ModelRenderer::Render()
{
	UpdateShaderData();

	mShader.Use();
	glBindVertexArray(mVaoID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetTextureId());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, Engine::GetInstance()->GetMaterialDataBuffer().GetTextureId());

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(mTextureMapping.mMapping[i].mTexture->GetTarget(), mTextureMapping.mMapping[i].mTexture->GetResourceId());
	}

	glUniform1i(mShader.GetUniform((int)EMainShaderUniformIndex::u_MaterialBaseIndex), GetMaterialBaseIndex());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, mDrawCmdCount, sizeof(Renderer::DrawElementsIndirectCommand));
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	//glDrawElementsInstanced(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0, (GLsizei)mObjs.GetCount());

	glBindVertexArray(0);
	mShader.UnUse();
}

void ModelRenderer::RenderWireFrame()
{
	UpdateShaderData();

	mWireFrameShader.Use();
	glBindVertexArray(mVaoID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetTextureId());

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetTextureId());

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_BUFFER, Engine::GetInstance()->GetMaterialDataBuffer().GetTextureId());

	//glUniform1i(mWireFrameShader.GetUniform((int)EMainShaderUniformIndex::u_MaterialBaseIndex), GetMaterialBaseIndex());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, mDrawCmdCount, sizeof(Renderer::DrawElementsIndirectCommand));
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	//glDrawElementsInstanced(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, 0, (GLsizei)mObjs.GetCount());

	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}



void ModelRenderer::SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, TextureIndex specularTextureIndex, GLfloat roughness, TextureIndex roughnessTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex)
{
	assert(materialIndex < mMaterialCount);

	if (materialIndex < mMaterialCount)
	{
		GLuint propertyIndex = Property_Per_Material * materialIndex;

		GLfloat * prop1 = mMaterials.GetProperty(propertyIndex);
		memcpy(prop1, glm::value_ptr(diffuse), sizeof(GLfloat) * 3);

		GLfloat * prop2 = mMaterials.GetProperty(propertyIndex + 1);
		memcpy(prop2, glm::value_ptr(specular), sizeof(GLfloat) * 3);

		GLbitfield roughnessBitfield = (uint32_t)(roughness * 32767);
		memcpy(&prop2[3], &roughnessBitfield, sizeof(GLfloat));

		GLfloat * prop3 = mMaterials.GetProperty(propertyIndex + 2);
		memcpy(prop3, glm::value_ptr(emissive), sizeof(GLfloat) * 3);

		MaterialTextureIndexes & texIndexes = mMaterialTextureIndexesList[materialIndex];
		texIndexes.mDiffuse = diffuseTextureIndex;
		texIndexes.mSpecular = specularTextureIndex;
		texIndexes.mRoughness = roughnessTextureIndex;
		texIndexes.mEmissive = emissiveTextureIndex;
		texIndexes.mNormal = normalTextureIndex;

		{
			PRINT_MESSAGE("\t\t- Material %i : Texture index (DSRNE) = (%i, %i, %i, %i, %i)", materialIndex, (int8_t)diffuseTextureIndex, (int8_t)specularTextureIndex, (int8_t)roughnessTextureIndex, (int8_t)normalTextureIndex, (int8_t)emissiveTextureIndex);
			PRINT_MESSAGE("\t\t\tDiffuse=(%f, %f, %f), Specular=(%f, %f, %f), Roughness=%f, Emissive=(%f, %f, %f)", diffuse.x, diffuse.y, diffuse.z, specular.x, specular.y, specular.z, roughness, emissive.x, emissive.y, emissive.z);
		}

	}
}

void ModelRenderer::SetMaterial(std::uint16_t materialIndex, const Renderer::MaterialDesc & mat)
{
	SetMaterial(materialIndex, mat.mDiffuse, mat.mDiffuseTextureIndex, mat.mSpecular, mat.mSpecularTextureIndex, mat.mRoughness, mat.mRoughnessTextureIndex, mat.mEmissive, mat.mEmissiveTextureIndex, mat.mNormalTextureIndex);
}

void ModelRenderer::SetMaterials(const Renderer::MaterialDescList & materials)
{
	PRINT_MESSAGE("\t* Material count : %li", materials.size());
	for (size_t i = 0; i < materials.size(); ++i)
	{
		SetMaterial((std::uint16_t)i, materials[i]);
	}
}

void ModelRenderer::UpdateMaterialTextureIndex()
{
	PRINT_MESSAGE("Update material texture index :");

	const TextureInfoList & texInfo = GetTextureInfoList();

	for (GLuint materialIndex = 0; materialIndex < (GLuint)mMaterialTextureIndexesList.size(); ++materialIndex)
	{
		MaterialTextureIndexes & texIndexes = mMaterialTextureIndexesList[materialIndex];

		std::uint8_t diffuseTextureIndex = texIndexes.mDiffuse != NoTexture ? (std::uint8_t)texInfo[texIndexes.mDiffuse].GetLayerIndex() : 0xFF;
		std::uint8_t specularTextureIndex = texIndexes.mSpecular != NoTexture ? (std::uint8_t)texInfo[texIndexes.mSpecular].GetLayerIndex() : 0xFF;
		std::uint8_t emissiveTextureIndex = texIndexes.mEmissive != NoTexture ? (std::uint8_t)texInfo[texIndexes.mEmissive].GetLayerIndex() : 0xFF;
		std::uint8_t normalTextureIndex = texIndexes.mNormal != NoTexture ? (std::uint8_t)texInfo[texIndexes.mNormal].GetLayerIndex() : 0xFF;
		std::uint8_t roughnessTextureIndex = texIndexes.mRoughness != NoTexture ? (std::uint8_t)texInfo[texIndexes.mRoughness].GetLayerIndex() : 0xFF;

		std::uint8_t diffuseSamplerIndex = texIndexes.mDiffuse != NoTexture ? (std::uint8_t)texInfo[texIndexes.mDiffuse].GetSamplerIndex() : 0xFF;
		std::uint8_t specularSamplerIndex = texIndexes.mSpecular != NoTexture ? (std::uint8_t)texInfo[texIndexes.mSpecular].GetSamplerIndex() : 0xFF;
		std::uint8_t emissiveSamplerIndex = texIndexes.mEmissive != NoTexture ? (std::uint8_t)texInfo[texIndexes.mEmissive].GetSamplerIndex() : 0xFF;
		std::uint8_t normalSamplerIndex = texIndexes.mNormal != NoTexture ? (std::uint8_t)texInfo[texIndexes.mNormal].GetSamplerIndex() : 0xFF;
		std::uint8_t roughnessSamplerIndex = texIndexes.mRoughness != NoTexture ? (std::uint8_t)texInfo[texIndexes.mRoughness].GetSamplerIndex() : 0xFF;

		PRINT_MESSAGE("\t- Material %i : (Sampler, Texture) Diffuse=(%i, %i), Specular=(%i, %i), Roughness=(%i, %i), Normal=(%i, %i), Emissive=(%i, %i)", materialIndex, (int8_t)diffuseSamplerIndex, (int8_t)diffuseTextureIndex, (int8_t)specularSamplerIndex, (int8_t)specularTextureIndex, (int8_t)roughnessSamplerIndex, (int8_t)roughnessTextureIndex, (int8_t)normalSamplerIndex, (int8_t)normalTextureIndex, (int8_t)emissiveSamplerIndex, (int8_t)emissiveTextureIndex);

		GLuint propertyIndex = Property_Per_Material * materialIndex;

		GLfloat * prop1 = mMaterials.GetProperty(propertyIndex);
		GLbitfield diffuseSpecularIndexes = ((diffuseSamplerIndex & 0xFF) << 24) | ((diffuseTextureIndex & 0xFF) << 16) | ((specularSamplerIndex & 0xFF) << 8) | (specularTextureIndex & 0xFF);
		memcpy(&prop1[3], &diffuseSpecularIndexes, sizeof(GLfloat));

		GLfloat * prop2 = mMaterials.GetProperty(propertyIndex + 1);
		GLbitfield roughnessBitfields;
		memcpy(&roughnessBitfields, &prop2[3], sizeof(GLfloat));
		roughnessBitfields = ((roughnessSamplerIndex & 0xFF) << 24) | ((roughnessTextureIndex & 0xFF) << 16) | (roughnessBitfields & 0xFFFF);
		memcpy(&prop2[3], &roughnessBitfields, sizeof(GLfloat));

		GLfloat * prop3 = mMaterials.GetProperty(propertyIndex + 2);
		GLbitfield emissiveNormalIndexes;
		emissiveNormalIndexes = ((emissiveSamplerIndex & 0xFF) << 24) | ((emissiveTextureIndex & 0xFF) << 16) | ((normalSamplerIndex & 0xFF) << 8) | (normalTextureIndex & 0xFF);
		memcpy(&prop3[3], &emissiveNormalIndexes, sizeof(GLfloat));
	}

	InitializeShaders();
}

void ModelRenderer::InitializeShaders()
{
	InitializeMainShader();
	InitializeWireFrameShader();
}

void ModelRenderer::InitializeMainShader()
{
	PRINT_MESSAGE("Initialize ModelRenderer shader....");

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/mesh.vs.glsl");
	mShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/mesh.gs.glsl");
	//mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/mesh.deferred.fs.glsl");

	PRINT_MESSAGE("Loading shader file : shaders/mesh.deferred.fs.glsl");
	// fragment shader
	std::vector<std::string> lightFsGlsl(2);
	Shader::MergeFile(lightFsGlsl[0], "shaders/mesh.deferred.fs.glsl");
	std::string & textureFuncSource = lightFsGlsl[1];
	Shader::GenerateTexGetFunction(textureFuncSource, (int)mTextureMapping.mMapping.size());
	mShader.LoadFromString(GL_FRAGMENT_SHADER, lightFsGlsl);

	const char * uniformNames[(int)EMainShaderUniformIndex::__uniforms_count__] =
	{
		"u_MaterialBaseIndex",
		"u_perInstanceDataSampler",
		//"u_materialIndexSampler",
		"u_materialDataSampler"
	};

	mShader.CreateAndLinkProgram();

	mShader.Use();
	mShader.AddUniforms(uniformNames, (int)EMainShaderUniformIndex::__uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mShader.GetUniform((int)EMainShaderUniformIndex::u_perInstanceDataSampler), 0);
	//glUniform1i(mShader.GetUniform((int)EMainShaderUniformIndex::u_materialIndexSampler), 1);
	glUniform1i(mShader.GetUniform((int)EMainShaderUniformIndex::u_materialDataSampler), 1);


	PRINT_MESSAGE("Texture mapping : sampler count = %li", mTextureMapping.mMapping.size());
	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		char uniformName[50];
		sprintf_s(uniformName, 50, "u_textureSampler[%i]", i);
		int uniformIndex = glGetUniformLocation(mShader.GetProgram(), uniformName); GL_CHECK_ERRORS;
		if (uniformIndex > 0)
		{
			glUniform1i(uniformIndex, i + FIRST_TEXTURE_SAMPLER_INDEX);	GL_CHECK_ERRORS;
			PRINT_MESSAGE("mapping : u_textureSampler[%i] = %i (uniform index = %i)", i, i + FIRST_TEXTURE_SAMPLER_INDEX, uniformIndex);
			//std::cout << "\t" << uniformName << " : " << uniformIndex << std::endl;
		}
	}

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE("... ModelRenderer shader initialized!");
}

void ModelRenderer::InitializeWireFrameShader()
{
	PRINT_MESSAGE("Initialize ModelRenderer (wireframe) shader....");

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/mesh.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_GEOMETRY_SHADER, "shaders/mesh.gs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/mesh.wireframe.fs.glsl");

	const char * uniformNames[(int)EWireFrameShaderUniformIndex::__uniforms_count__] =
	{
		//"u_MaterialBaseIndex",
		"u_perInstanceDataSampler",
		//"u_materialIndexSampler"
	};

	mWireFrameShader.CreateAndLinkProgram();

	mWireFrameShader.Use();
	mWireFrameShader.AddUniforms(uniformNames, (int)EWireFrameShaderUniformIndex::__uniforms_count__);

	//pass values of constant uniforms at initialization
	glUniform1i(mWireFrameShader.GetUniform((int)EWireFrameShaderUniformIndex::u_perInstanceDataSampler), 0);
	//glUniform1i(mWireFrameShader.GetUniform((int)EWireFrameShaderUniformIndex::u_materialIndexSampler), 1);

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....ModelRenderer (wireframe) shader initialized!");
}

void ModelRenderer::UpdateShaderData()
{
	if (!mIsShaderBufferSet)
	{
		// --------------------------------------------

		//glBindBuffer(GL_TEXTURE_BUFFER, mMaterialIndexBuffer.GetBufferId());

		//GLuint * matIndexBuffer = (GLuint *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		//assert(matIndexBuffer != nullptr);

		//if (matIndexBuffer != nullptr)
		//{
		//	for(GLsizei i = 0; i < mDrawCmdCount; ++i)
		//	{
		//		matIndexBuffer[i] = mMeshDrawInstanceList[i].mBaseInstance;
		//	}

		//	glUnmapBuffer(GL_TEXTURE_BUFFER);
		//}

		// --------------------------------------------

		glBindBuffer(GL_TEXTURE_BUFFER, mModelMatrixBuffer.GetBufferId());
		std::uint8_t * buffer = (std::uint8_t *)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
		mObjs.ForEach([this, &buffer](Renderables::Model* obj)
		{
			if (obj->GetFrame()->IsModified())
			{
				memcpy(buffer, glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetRealPart()), sizeof(glm::quat));
				memcpy(buffer + sizeof(glm::quat), glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetDualPart()), sizeof(glm::quat));
				obj->GetFrame()->SetIsModified(false);
			}
			buffer += sizeof(PerInstanceData);
		});
		glUnmapBuffer(GL_TEXTURE_BUFFER);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

		// --------------------------------------------

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);

		Renderer::DrawElementsIndirectCommand * cmd = (Renderer::DrawElementsIndirectCommand *)	glMapBuffer(GL_DRAW_INDIRECT_BUFFER, GL_WRITE_ONLY);

		GLuint instanceCount = (GLuint)mObjs.GetCount();
		for (auto i = 0; i < mDrawCmdCount; i++)
		{
			cmd->mInstanceCount = instanceCount;
			++cmd;
		}

		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		// --------------------------------------------

		mIsShaderBufferSet = true;
	}
}


Renderables::Model * ModelRenderer::CreateModelInstance(std::uint8_t materialGroupIndex)
{
	if (GetCount() < GetCapacity())
	{
		Renderables::Model *obj = new Renderables::Model(materialGroupIndex);
		mObjs.Attach(obj);
		mIsShaderBufferSet = false;
		return obj;
	}
	else
		return nullptr;
}

void ModelRenderer::DeleteModelInstance(Renderables::Model * modelInstance)
{
	if (modelInstance == nullptr)
		return;
	mObjs.Detach(modelInstance);
	SAFE_DELETE(modelInstance);
	mIsShaderBufferSet = false;
}

ModelRenderer * ModelRenderer::CreateFromFile(Engine * engine, const std::string & modelFilePath, const std::string & textureBasePath, const Geometry::ModelData::LoadOptions & options, size_t capacity, size_t pageSize)
{
	Geometry::ModelData model;
	model.LoadModel(modelFilePath, textureBasePath, options);
	if (model.IsLoaded())
	{
		return CreateFromModel(engine, model, capacity, pageSize);
	}
	else
	{
		PRINT_ERROR("Cannot create the 'ModelRenderer' from model file '%s' : model loading has failed!", modelFilePath.c_str());
		return nullptr;
	}
}

ModelRenderer * ModelRenderer::CreateFromModel(Engine * engine, const Geometry::ModelData & model, size_t capacity, size_t pageSize)
{
	if (model.IsLoaded())
	{
		ModelRenderer * renderer = new ModelRenderer(capacity, pageSize);

		engine->AttachRenderer(renderer);

		renderer->SetModel(model.GetVertexList(), model.GetIndexList(), model.GetMaterialDescList(), model.GetTextureDescList(), model.GetMeshDrawInstanceList());

		return renderer;
	}
	else
	{
		PRINT_ERROR("Cannot create the 'ModelRenderer' from model : the model is not loaded!");
		return nullptr;
	}
}

	} // namespace Renderers
} // namespace CoreFx