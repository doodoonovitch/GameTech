#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{




ModelRenderer::ModelRenderer(size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Model, 3>(capacity, pageSize, "ModelRenderer", "ModelWireFrameRenderer")
	, mMaterialCount(0)
	, mDrawCmdCount(0)
	, mMaterialTextureIndexesList(0)
	, mIsModelSet(false)
	, mIsShaderBufferSet(false)
	, mUpdateObjMatrixIndexProp(false)
{

}

void ModelRenderer::SetModel(const Renderer::VertexDataVector & vertexList, const Renderer::IndexVector & indexList, const Renderer::MaterialDescList & materialDescList, const Renderer::TextureDescList & textureDescList, const Renderer::DrawElementsIndirectCommandList & meshDrawInstanceList, const Geometry::ModelData::ModelMappingList & modelMapping)
{
	if (mIsModelSet)
	{
		PRINT_ERROR("[ModelRenderer] The renderer is already initialized with a model!");
		return;
	}

	PRINT_BEGIN_SECTION;
	PRINT_MESSAGE("Initialize ModelRenderer....");
	
	mModelInstanceMappingList.clear();
	mModelInstanceMappingList.reserve(modelMapping.size());
	for (Geometry::ModelData::ModelMappingList::const_iterator it = modelMapping.begin(); it != modelMapping.end(); ++it)
	{
		mModelInstanceMappingList.push_back(*it);
	}

	mMaterialCount = (GLuint)materialDescList.size();
	mDrawCmdCount = (GLsizei)meshDrawInstanceList.size();
	mMaterialTextureIndexesList.resize((GLuint)materialDescList.size());
	mDrawCommandList = meshDrawInstanceList;

	mShaderMaterialList.resize(mMaterialCount);

	AddTextures(textureDescList);
	SetMaterials(materialDescList);
	BuildTextureMapping();

	const GLsizei vertexDataSize = sizeof(Renderer::VertexData);

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

		if (!Engine::GetInstance()->IsUsedExtensionSupported(Engine::ARB_shader_draw_parameters))
		{
			glEnableVertexAttribArray(Shader::MESHID_ATTRIBUTE);
			glVertexAttribIPointer(Shader::MESHID_ATTRIBUTE, 1, GL_UNSIGNED_INT, vertexDataSize, (GLvoid*)offsetof(Renderer::VertexData, mMeshId));
			GL_CHECK_ERRORS;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[VBO_Index]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(sizeof(GLuint) * indexList.size()), indexList.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	//	 
	glBindVertexArray(0);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(Renderer::DrawElementsIndirectCommand) * mDrawCmdCount, mDrawCommandList.data(), GL_STATIC_DRAW);
	GL_CHECK_ERRORS;
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	GL_CHECK_ERRORS;

	LoadTextures();
	UpdateMaterialTextureIndex();
	InitializeShaders();

	mPrecomputeDataBuffer.CreateResource(GL_DYNAMIC_COPY, GetCapacity() * cPerInstanceDataSize, nullptr);
	mLocationRawDataBuffer.CreateResource(GL_STATIC_DRAW, GetCapacity() * cPerInstanceDataSize, nullptr);
	mMaterialIndexBuffer.CreateResource(GL_STATIC_DRAW, mMaterialCount * sizeof(GLuint), nullptr);
	mMaterialBuffer.CreateResource(GL_STATIC_DRAW, mMaterialCount * cShaderMaterialSize, nullptr/*mShaderMaterialList.data()*/);

	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mMaterialBuffer);
	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mPrecomputeDataBuffer);
	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mLocationRawDataBuffer);
	PRINT_GEN_SHADERSTORAGEBUFFER("[ModelRenderer]", mMaterialIndexBuffer);

	mIsInitialized = true;

	PRINT_MESSAGE("... ModelRenderer initialized!\n");
	PRINT_END_SECTION;
}


ModelRenderer::~ModelRenderer()
{
}
 

void ModelRenderer::Update()
{
	UpdateShaderData();
	Engine * engine = Engine::GetInstance();
	const GLuint itemSize = sizeof(PerInstanceData) / sizeof(glm::vec4);
	engine->ComputeViewTransform(mLocationRawDataBuffer.GetBufferId(), mPrecomputeDataBuffer.GetBufferId(), (GLuint)mObjs.GetCount(), itemSize, itemSize);
}

void ModelRenderer::Render()
{
	UpdateShaderData();

	mShader.Use();
	glBindVertexArray(mVaoID);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataBuffer, mPrecomputeDataBuffer.GetBufferId());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataIndexBuffer, mMaterialIndexBuffer.GetBufferId());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_Materials, mMaterialBuffer.GetBufferId());

	for (int i = 0; i < (int)mTextureMapping.mMapping.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + FIRST_TEXTURE_SAMPLER_INDEX + i);
		glBindTexture(mTextureMapping.mMapping[i].mTexture->GetTarget(), mTextureMapping.mMapping[i].mTexture->GetResourceId());
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, mDrawCmdCount, sizeof(Renderer::DrawElementsIndirectCommand));
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glBindVertexArray(0);
	mShader.UnUse();
}

void ModelRenderer::RenderWireFrame()
{
	UpdateShaderData();

	mWireFrameShader.Use();
	glBindVertexArray(mVaoID);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataBuffer, mPrecomputeDataBuffer.GetBufferId());

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, u_PerInstanceDataIndexBuffer, mMaterialIndexBuffer.GetBufferId());

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, mDrawCmdCount, sizeof(Renderer::DrawElementsIndirectCommand));
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glBindVertexArray(0);
	mWireFrameShader.UnUse();
}



void ModelRenderer::SetMaterial(std::uint16_t materialIndex, const glm::vec3& diffuse, TextureIndex diffuseTextureIndex, const glm::vec3& specular, TextureIndex specularTextureIndex, GLfloat roughness, TextureIndex roughnessTextureIndex, const glm::vec3& emissive, TextureIndex emissiveTextureIndex, TextureIndex normalTextureIndex)
{
	assert(materialIndex < mMaterialCount);

	if (materialIndex < mMaterialCount)
	{
		ShaderMaterial & mat = mShaderMaterialList[materialIndex];

		mat.mDiffuse = diffuse;
		mat.mSpecular = specular;
		mat.mEmissive = emissive;

		mat.mRoughness = roughness;

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
		ShaderMaterial & mat = mShaderMaterialList[materialIndex];

		mat.mDiffuseTextureIndex	= texIndexes.mDiffuse != NoTexture ? (GLint)texInfo[texIndexes.mDiffuse].GetLayerIndex() : -1;
		mat.mDiffuseSamplerIndex	= texIndexes.mDiffuse != NoTexture ? (GLint)texInfo[texIndexes.mDiffuse].GetSamplerIndex() : -1;

		mat.mSpecularTextureIndex	= texIndexes.mSpecular != NoTexture ? (GLint)texInfo[texIndexes.mSpecular].GetLayerIndex() : -1;
		mat.mSpecularSamplerIndex	= texIndexes.mSpecular != NoTexture ? (GLint)texInfo[texIndexes.mSpecular].GetSamplerIndex() : -1;

		mat.mEmissiveTextureIndex	= texIndexes.mEmissive != NoTexture ? (GLint)texInfo[texIndexes.mEmissive].GetLayerIndex() : -1;
		mat.mEmissiveSamplerIndex	= texIndexes.mEmissive != NoTexture ? (GLint)texInfo[texIndexes.mEmissive].GetSamplerIndex() : -1;

		mat.mNormalTextureIndex		= texIndexes.mNormal != NoTexture ? (GLint)texInfo[texIndexes.mNormal].GetLayerIndex() : -1;
		mat.mNormalSamplerIndex		= texIndexes.mNormal != NoTexture ? (GLint)texInfo[texIndexes.mNormal].GetSamplerIndex() : -1;

		mat.mRoughnessTextureIndex	= texIndexes.mRoughness != NoTexture ? (GLint)texInfo[texIndexes.mRoughness].GetLayerIndex() : -1;
		mat.mRoughnessSamplerIndex	= texIndexes.mRoughness != NoTexture ? (GLint)texInfo[texIndexes.mRoughness].GetSamplerIndex() : -1;

		PRINT_MESSAGE("\t- Material %i : (Sampler, Texture) Diffuse=(%i, %i), Specular=(%i, %i), Roughness=(%i, %i), Normal=(%i, %i), Emissive=(%i, %i)", materialIndex, mat.mDiffuseSamplerIndex, mat.mDiffuseTextureIndex, mat.mSpecularSamplerIndex, mat.mSpecularTextureIndex, mat.mRoughnessSamplerIndex, mat.mRoughnessTextureIndex, mat.mNormalSamplerIndex, mat.mNormalTextureIndex, mat.mEmissiveSamplerIndex, mat.mEmissiveTextureIndex);
	}
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

	mShader.CreateAndLinkProgram();

	mShader.Use();


	//const char * uniformNames[(int)EMainShaderUniformIndex::__uniforms_count__] =
	//{
	//};
	//mShader.AddUniforms(uniformNames, (int)EMainShaderUniformIndex::__uniforms_count__);

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

	mWireFrameShader.CreateAndLinkProgram();

	mWireFrameShader.Use();

	mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	PRINT_MESSAGE(".....ModelRenderer (wireframe) shader initialized!");
}

void ModelRenderer::UpdateShaderData()
{
	if (!mIsShaderBufferSet)
	{
		PRINT_MESSAGE("[ModelRenderer] *********************  Update shader buffers !  *********************");

		GLenum target = GL_SHADER_STORAGE_BUFFER;

		// --------------------------------------------

		glBindBuffer(target, mMaterialBuffer.GetBufferId());
		GLuint * matBuffer = (GLuint *)glMapBuffer(target, GL_WRITE_ONLY);
		const std::uint8_t * data = (const std::uint8_t *)mShaderMaterialList.data();
		memcpy(matBuffer, data, mMaterialCount * cShaderMaterialSize);
		glUnmapBuffer(target);
		glBindBuffer(target, 0);

		// --------------------------------------------

		glBindBuffer(target, mMaterialIndexBuffer.GetBufferId());

		GLuint index = 0;
		GLuint * matIndexBuffer = (GLuint *)glMapBuffer(target, GL_WRITE_ONLY);
		assert(matIndexBuffer != nullptr);
#ifdef _DEBUG
		std::vector<GLuint> indexList(mMaterialCount);
#endif
		if (matIndexBuffer != nullptr)
		{
			for(ModelInstanceMappingList::const_iterator it = mModelInstanceMappingList.begin(); it !=  mModelInstanceMappingList.end(); ++it)
			{
				const ModelInstanceMapping & mapping = *it;
				for (GLuint j = 0; j < mapping.mDrawCommandCount; ++j)
				{
					const Renderer::DrawElementsIndirectCommand & cmd = mDrawCommandList[mapping.mDrawCommandIndex + j];
#ifdef _DEBUG
					indexList[cmd.mBaseInstance] = index;
#endif
					matIndexBuffer[cmd.mBaseInstance] = index;
				}
				index += mapping.mInstanceCount;
			}

			glUnmapBuffer(target);
			glBindBuffer(target, 0);
		}

		// --------------------------------------------

		if (mUpdateObjMatrixIndexProp)
		{
			UpdateObjMaxtrixIndexProp();
		}

		// --------------------------------------------

		glBindBuffer(target, mLocationRawDataBuffer.GetBufferId());
		std::uint8_t * bufferBase = (std::uint8_t *)glMapBuffer(target, GL_WRITE_ONLY);
		mObjs.ForEach([this, bufferBase](Renderables::Model* obj)
		{
			GLuint matrixIndex = obj->GetMatrixIndex();
			std::uint8_t * buffer = bufferBase + (matrixIndex * sizeof(PerInstanceData));
			if (mUpdateObjMatrixIndexProp || obj->GetFrame()->IsModified())
			{
				memcpy(buffer, glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetRealPart()), sizeof(glm::quat));
				memcpy(buffer + sizeof(glm::quat), glm::value_ptr(obj->GetFrame()->GetDualQuaternion().GetDualPart()), sizeof(glm::quat));
				memcpy(buffer + sizeof(glm::quat) + sizeof(glm::quat), glm::value_ptr(obj->GetFrame()->GetScale()), sizeof(glm::vec3));
				obj->GetFrame()->SetIsModified(false);
			}
		});
		glUnmapBuffer(target);
		glBindBuffer(target, 0);

		mUpdateObjMatrixIndexProp = false;
		// --------------------------------------------

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mVboIDs[VBO_Indirect]);

		Renderer::DrawElementsIndirectCommand * cmd = (Renderer::DrawElementsIndirectCommand *)	glMapBuffer(GL_DRAW_INDIRECT_BUFFER, GL_WRITE_ONLY);

		for (auto i = 0; i < mDrawCmdCount; i++)
		{
			cmd->mInstanceCount = mDrawCommandList[i].mInstanceCount;
			++cmd;
		}

		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		// --------------------------------------------

		mIsShaderBufferSet = true;
	}
}

void ModelRenderer::UpdateObjMaxtrixIndexProp()
{
	GLuint index = 0;
	std::vector<GLuint> indexList(mModelInstanceMappingList.size(), 0);
	for (GLsizei i = 0; i < (GLsizei)mModelInstanceMappingList.size(); ++i)
	{
		indexList[i] = index;
		index += mModelInstanceMappingList[i].mInstanceCount;
	}

	mObjs.ForEach([this, &indexList](Renderables::Model* obj)
	{
		GLuint & offset = indexList[obj->GetModelIndex()];
		obj->SetMatrixIndex(offset);
		++offset;
	});
}

void ModelRenderer::UpdateDrawCommandListInstanceCount(const ModelInstanceMapping & mapping)
{
	for (GLuint i = 0; i < mapping.mDrawCommandCount; ++i)
	{
		Renderer::DrawElementsIndirectCommand & drawCommand = mDrawCommandList[mapping.mDrawCommandIndex + i];
		drawCommand.mInstanceCount = mapping.mInstanceCount;
	}
}

Renderables::Model * ModelRenderer::CreateModelInstance(GLuint modelIndex)
{
	if (modelIndex >= mModelInstanceMappingList.size())
	{
		PRINT_ERROR("Cannot create model instance : invalid modelIndex=%li! (Model count=%li)", modelIndex, mModelInstanceMappingList.size());
		return nullptr;
	}
		
	size_t count = GetCount();
	size_t capacity = GetCapacity();
	if (count < capacity)
	{
		Renderables::Model *obj = new Renderables::Model(modelIndex);
		mObjs.Attach(obj);

		ModelInstanceMapping & mapping = mModelInstanceMappingList[modelIndex];
		++mapping.mInstanceCount;
		UpdateDrawCommandListInstanceCount(mapping);

		mIsShaderBufferSet = false;
		mUpdateObjMatrixIndexProp = true;
		return obj;
	}
	else
	{
		PRINT_ERROR("Cannot create model instance : out of capacity (capacity = %li, count = %li)!", GetCapacity(), GetCount());
		return nullptr;
	}		
}

void ModelRenderer::DeleteModelInstance(Renderables::Model * modelInstance)
{
	if (modelInstance == nullptr)
		return;

	GLuint modelIndex = modelInstance->GetModelIndex();
	ModelInstanceMapping & mapping = mModelInstanceMappingList[modelIndex];
	--mapping.mInstanceCount;
	UpdateDrawCommandListInstanceCount(mapping);

	mObjs.Detach(modelInstance);
	SAFE_DELETE(modelInstance);
	mIsShaderBufferSet = false;
	mUpdateObjMatrixIndexProp = true;
}

ModelRenderer * ModelRenderer::CreateFromFile(Engine * engine, const std::string & modelFilePath, const std::string & textureBasePath, const Geometry::ModelData::LoadOptions & options, size_t capacity, size_t pageSize)
{
	Geometry::ModelData model;
	bool isLoaded = model.LoadModel(modelFilePath, textureBasePath, options);
	if (isLoaded)
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
	ModelRenderer * renderer = new ModelRenderer(capacity, pageSize);

	engine->AttachRenderer(renderer);

	renderer->SetModel(model.GetVertexList(), model.GetIndexList(), model.GetMaterialDescList(), model.GetTextureDescList(), model.GetMeshDrawInstanceList(), model.GetModelMappingList());

	return renderer;
}

	} // namespace Renderers
} // namespace CoreFx