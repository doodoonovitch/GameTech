#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderables
	{




Mesh::Mesh(const Geometry::MeshData& meshData)
{
	void* baseptr;
	ptrdiff_t position, normal, tangent, binormal, uv;
	int vertexDataSize;

	meshData.GetVertexComponentOffsets(baseptr, position, normal, tangent, binormal, uv, vertexDataSize);

	//setup shader
	std::vector<std::string> vertexShaderFiles, fragmentShaderFiles;
	vertexShaderFiles.push_back("shader/mesh_material_def.inc");

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
	mShader.Use();

		mShader.AddAttribute("vVertex");
		if (normal != -1)
			mShader.AddAttribute("vNormal");
		if (tangent != -1)
			mShader.AddAttribute("vTangent");
		if (binormal != -1)
			mShader.AddAttribute("vBinormal");
		if (uv != -1)
			mShader.AddAttribute("vUV");

		mShader.AddUniform("mMVP");
	//pass values of constant uniforms at initialization
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup vao and vbo stuff
	glGenVertexArrays(1, &mVaoID);
	glGenBuffers(mVboCount, mVboIDs);
	 
	glBindVertexArray(mVaoID);

		glBindBuffer (GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, meshData.GetVertexCount(), baseptr, GL_STATIC_DRAW);
		GL_CHECK_ERRORS;
		 
		glEnableVertexAttribArray(mShader["vVertex"]);
		glVertexAttribPointer(mShader["vVertex"], 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)position);
		GL_CHECK_ERRORS;

		if (normal != -1)
		{
			glEnableVertexAttribArray(mShader["vNormal"]);
			glVertexAttribPointer(mShader["vNormal"], 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)normal);
			GL_CHECK_ERRORS;
		}

		if (tangent != -1)
		{
			glEnableVertexAttribArray(mShader["vTangent"]);
			glVertexAttribPointer(mShader["vTangent"], 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)tangent);
			GL_CHECK_ERRORS;
		}

		if (binormal != -1)
		{
			glEnableVertexAttribArray(mShader["vBinormal"]);
			glVertexAttribPointer(mShader["vBinormal"], 3, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)binormal);
			GL_CHECK_ERRORS;
		}

		if (uv != -1)
		{
			glEnableVertexAttribArray(mShader["vUV"]);
			glVertexAttribPointer(mShader["vUV"], 2, GL_FLOAT, GL_FALSE, vertexDataSize, (const GLvoid*)uv);
			GL_CHECK_ERRORS;
		}

	//	 
	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	meshData.CopySubMeshDescList(mSubMeshDescList);
	meshData.CopyMaterialList(mMaterialList);
}


Mesh::~Mesh()
{
}

 
//void Mesh::Render(glm::mat4 const & VP)
//{
//	glm::mat4 M = GetFrame()->GetMatrix();
//	glm::mat4 MVP = VP * M;
//
//	mShader.Use();
//		glUniformMatrix4fv(mShader("mMVP"), 1, GL_FALSE, glm::value_ptr(MVP));
//		glBindVertexArray(mVaoID);
//		for (auto it : mSubMeshDescList)
//		{
//			glDrawArrays(GL_TRIANGLES, it.mFirstIndex, it.mCount);
//		}
//		glBindVertexArray(0);
//	mShader.UnUse();
//}



	} // namespace Renderables
} // namespace CoreFx