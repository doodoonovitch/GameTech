#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



SkydomeRenderer::SkydomeRenderer(const std::string & skyCubeMapTextureFilename, const int rings, const int segments)
		: RendererHelper<2>(0, "SkydomeRenderer", "SkydomeWireFrameRenderer", Renderer::Forward_Pass)
		, mCubeMapTexture(Engine::GetInstance()->GetTextureManager()->LoadTextureCubeMap(skyCubeMapTextureFilename))
{
	std::cout << std::endl;
	std::cout << "Initialize SkydomeRenderer...." << std::endl;

	const char * uniformNames[__uniforms_count__] =
	{
		"u_SkyboxCubeMapSampler"
	};

	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.forward.fs.glsl");
	//mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddUniforms(uniformNames, __uniforms_count__);

		glUniform1i(mShader.GetUniform(u_SkyboxCubeMapSampler), 0); GL_CHECK_ERRORS;

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();
	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	std::vector<glm::vec3> vertices;
	std::vector<GLushort> indices;

	CreateGeometry(vertices, indices, 5160.0f, 1000.0f, rings, segments);
	mIndiceCount = (GLsizei)indices.size();

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndiceCount, indices.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	std::cout << "... SkydomeRenderer initialized!" << std::endl << std::endl;
}

SkydomeRenderer::~SkydomeRenderer()
{
}

void SkydomeRenderer::CreateGeometry(std::vector<glm::vec3> & vertices, std::vector<GLushort> & indices, GLfloat radius_, const GLfloat height_, const int rings_, const int segments_)
{
	// Destroy current instance

	vertices.clear();
	indices.clear();

	// Clamp arguments
	const GLfloat Epsilon = glm::epsilon<float>();
	GLfloat radius = max(radius_, Epsilon);
	GLfloat height = max(height_, Epsilon);

	int rings = max(rings_, 1) + 1;
	int segments = max(segments_, 4) + 1;

	// Init vertex data container

	vertices.reserve(rings * (segments + 1));

	// Fill vertex data container

	for (int y = 0; y <= segments; ++y)
	{
		GLfloat theta = static_cast<GLfloat>(y) / segments * glm::two_pi<GLfloat>();

		for (int x = rings - 1; x >= 0; --x)
		{
			GLfloat phi = static_cast< GLfloat >(x) / rings * glm::half_pi<GLfloat>();

			vertices.push_back(glm::vec3(static_cast< GLfloat >(sin(phi) * cos(theta) * radius)
				, static_cast< GLfloat >(cos(phi) * height)
				, static_cast< GLfloat >(sin(phi) * sin(theta) * radius)));
		}
	}

	// Init index container

	indices.reserve(segments * rings * 2);

	// Fill index container

	bool leftToRight = false;

	for (int y = 0; y < segments; ++y)
	{
		if (true == leftToRight)
		{
			for (int x = 0; x < rings; ++x)
			{
				indices.push_back((GLushort)(y      * rings + x));
				indices.push_back((GLushort)((y + 1) * rings + x));
			}
		}
		else
		{
			for (int x = rings - 1; x >= 0; --x)
			{
				indices.push_back((GLushort)((y + 1) * rings + x));
				indices.push_back((GLushort)(y      * rings + x));
			}
		}

		leftToRight = !leftToRight;
	}
}



void SkydomeRenderer::Render()
{
	mShader.Use();
		glBindVertexArray(mVaoID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(mCubeMapTexture->GetTarget(), mCubeMapTexture->GetResourceId());
			glDrawElements(GL_TRIANGLE_STRIP, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);
		glBindVertexArray(0);
	mShader.UnUse();
}
 
void SkydomeRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);
			glDrawElements(GL_TRIANGLE_STRIP, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);
		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}



	} // namespace Renderers
} // namespace CoreFx