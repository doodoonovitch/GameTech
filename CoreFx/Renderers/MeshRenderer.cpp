#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{




MeshRenderer::MeshRenderer(const Geometry::MeshData& meshData, size_t capacity, size_t pageSize)
	: SceneObjectRenderer<Renderables::Mesh, 1>(capacity, pageSize)
{
	void* baseptr;
	ptrdiff_t position, normal, tangent, binormal, uv;
	int vertexDataSize;

	meshData.GetVertexComponentOffsets(baseptr, position, normal, tangent, binormal, uv, vertexDataSize);

	//setup shader
	std::vector<std::string> vertexShaderFiles, fragmentShaderFiles;
	//vertexShaderFiles.push_back("shader/mesh_material_def.inc");

	switch (meshData.GetVertexType())
	{
	case Geometry::VertexType::Position:
		vertexShaderFiles.push_back("shaders/mesh_shader.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader.frag");
		break;

	case Geometry::VertexType::PositionNormal:
		vertexShaderFiles.push_back("shaders/mesh_shader_N.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_N.frag");
		break;

	case Geometry::VertexType::PositionNormalTangent:
		vertexShaderFiles.push_back("shaders/mesh_shader_NT.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_NT.frag");
		break;

	case Geometry::VertexType::PositionNormalTangentBinormal:
		vertexShaderFiles.push_back("shaders/mesh_shader_NTB.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_NTB.frag");
		break;

	case Geometry::VertexType::PositionUV:
		vertexShaderFiles.push_back("shaders/mesh_shader_UV.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_UV.frag");
		break;

	case Geometry::VertexType::PositionNormalUV:
		vertexShaderFiles.push_back("shaders/mesh_shader_NUV.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_NUV.frag");
		break;

	case Geometry::VertexType::PositionNormalTangentUV:
		vertexShaderFiles.push_back("shaders/mesh_shader_NTUV.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_NTUV.frag");
		break;

	case Geometry::VertexType::PositionNormalTangentBinormalUV:
		vertexShaderFiles.push_back("shaders/mesh_shader_NTBUV.vert");
		fragmentShaderFiles.push_back("shaders/mesh_shader_NTBUV.frag");
		break;
	}

	mShader.LoadFromFile(GL_VERTEX_SHADER, vertexShaderFiles);
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, fragmentShaderFiles);

	mShader.CreateAndLinkProgram();
	//mShader.Use();
		//mShader.AddAttribute("vPosition");
		//if (normal != -1)
		//	mShader.AddAttribute("vNormal");
		//if (tangent != -1)
		//	mShader.AddAttribute("vTangent");
		//if (binormal != -1)
		//	mShader.AddAttribute("vBinormal");
		//if (uv != -1)
		//	mShader.AddAttribute("vUV");
		//pass values of constant uniforms at initialization
	//mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);
	 
	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, meshData.GetVertexCount(), baseptr, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)position);
		GL_CHECK_ERRORS;

		if (normal != -1)
		{
			glEnableVertexAttribArray(Shader::NORMAL_ATTRIBUTE); // mShader["vNormal"]
			glVertexAttribPointer(Shader::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)normal);
			GL_CHECK_ERRORS;
		}

		if (tangent != -1)
		{
			glEnableVertexAttribArray(Shader::TANGENT_ATTRIBUTE);
			glVertexAttribPointer(Shader::TANGENT_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)tangent);
			GL_CHECK_ERRORS;
		}

		if (binormal != -1)
		{
			glEnableVertexAttribArray(Shader::BINORMAL_ATTRIBUTE);
			glVertexAttribPointer(Shader::BINORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)binormal);
			GL_CHECK_ERRORS;
		}

		if (uv != -1)
		{
			glEnableVertexAttribArray(Shader::UV_ATTRIBUTE);
			glVertexAttribPointer(Shader::UV_ATTRIBUTE, 2, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)uv);
			GL_CHECK_ERRORS;
		}

	//	 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	meshData.CopySubMeshDescList(mSubMeshDescList);
	meshData.CopyMaterialList(mMaterialList);
}


MeshRenderer::~MeshRenderer()
{
}

 
void MeshRenderer::Render()
{
	//mShader.Use();
	//	glBindVertexArray(mVaoID);
	//	for (auto it : mSubMeshDescList)
	//	{
	//		glDrawArrays(GL_TRIANGLES, it.mFirstIndex, it.mCount);
	//	}
	//	glBindVertexArray(0);
	//mShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx