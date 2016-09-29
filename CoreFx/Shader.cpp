#include "stdafx.h"
#include "CoreFx.h"

using namespace std;

namespace CoreFx
{

	std::string Shader::sRendererShadersCommon;
	std::string Shader::sComputeShadersCommon;


Shader::Shader(const char * title)
	: mProgram(0)
	, mTitle(title != nullptr ? title : "")
{
	mAttributeList.clear();
	mUniformLocationList.clear();
}

Shader::~Shader(void)
{
	DeleteShaderProgram();
	mAttributeList.clear();
	mUniformLocationList.clear();
	mShaders.clear();
}

void Shader::DeleteShaderProgram()
{
	glDeleteProgram(mProgram);
	mProgram = 0;
}

const char* Shader::ShaderName(GLenum shaderType)
{
	switch (shaderType)
	{
	case GL_COMPUTE_SHADER:
		return "Compute shader";

	case GL_VERTEX_SHADER:
		return "Vertex shader";

	case GL_TESS_CONTROL_SHADER:
		return "Tess. control shader";

	case GL_TESS_EVALUATION_SHADER:
		return "Tess evaluation shader";

	case GL_GEOMETRY_SHADER:
		return "Geometry shader";

	case GL_FRAGMENT_SHADER:
		return "Fragment shader";

	default:
		return "[Unknown shader type]";
	}
}

const char * Shader::GetCommonIncludeFilename(GLuint includeFileId)
{
	switch (includeFileId)
	{
	case (GLuint)EIncludeCommon::RendererShadersHeader:
		return "shaders/RendererShaders.header.glsl";

	case (GLuint)EIncludeCommon::ComputeShadersHeader:
		return "shaders/ComputeShaders.header.glsl";

	case (GLuint)EIncludeCommon::Quaternions:
		return "shaders/Quaternions.incl.glsl";

	case (GLuint)EIncludeCommon::FrameCommon:
		return "shaders/RenderersCommon.incl.glsl";

	default:
		return nullptr;
	}
}

bool Shader::LoadCommonInclude(std::string & buffer, EInclude includes)
{
	for (int i = 0; i < 4; ++i)
	{
		GLuint item = 1 << i;
		if (((GLuint)includes & item) != 0)
		{
			const char * filename = GetCommonIncludeFilename(item);
			assert(filename != nullptr);
			if (filename != nullptr)
			{
				if (!MergeFile(buffer, filename))
				{
					PRINT_ERROR("Error loading common include file : '%s' !", filename, 0);
					return false;
				}
			}
		}
	}

	return true;
}

void Shader::LoadFromString(GLenum whichShader, const std::vector<std::string> & sources, EInclude includes)
{
	bool includeCommon = includes != EInclude::None;
	std::vector<const char *> tmp;
	tmp.reserve(sources.size() + (includeCommon ? 1 : 0));

	switch (includes)
	{
	case EInclude::RendererShadersCommon:
		{
			if (sRendererShadersCommon.empty())
			{
				sRendererShadersCommon.append("#version 440\n");
				if (Engine::GetInstance()->IsUsedExtensionSupported(Engine::ARB_shader_draw_parameters))
				{
					sRendererShadersCommon.append("#define ARB_SHADER_DRAW_PARAMETERS 1 \n");
				}

				if (!LoadCommonInclude(sRendererShadersCommon, includes))
				{
					return;
				}
			}
			tmp.push_back(sRendererShadersCommon.c_str());
		}
		break;

	case EInclude::ComputeShadersCommon:
		{
			if (sComputeShadersCommon.empty())
			{
				sComputeShadersCommon.append("#version 440\n");

				if (!LoadCommonInclude(sComputeShadersCommon, includes))
				{
					return;
				}
			}
			tmp.push_back(sComputeShadersCommon.c_str());
		}
		break;

	case EInclude::None:
	default:
		break;
	}


	GLuint shader = glCreateShader(whichShader);

	for (auto it = sources.begin(); it != sources.end(); ++it)
	{
		tmp.push_back(it->c_str());
	}

	const char ** srcList = tmp.data();
	glShaderSource(shader, (GLsizei)tmp.size(), srcList, NULL);
	GL_CHECK_ERRORS;

	//check whether the shader loads fine
	GLint status;
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		infoLog[infoLogLength] = 0;
		PRINT_ERROR("%s Compile log : %s", ShaderName(whichShader), infoLog);
		delete[] infoLog;
	}
	mShaders.push_back(shader);
}

void Shader::LoadFromFile(GLenum whichShader, const std::string& filename, EInclude includes)
{
	std::vector<string> buffers(1);
	string & buffer = buffers.back();

	PRINT_MESSAGE("Loading shader file : '%s'.", filename.c_str());
	if (MergeFile(buffer, filename))
	{
		LoadFromString(whichShader, buffers, includes);
	}
	else
	{
		PRINT_ERROR("Error loading shader : '%s'!", filename.c_str());
	}
}

void Shader::LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames, EInclude includes)
{
	std::vector<string> buffers(filenames.size());

	int i = 0;
	for (auto it : filenames)
	{		
		PRINT_MESSAGE("Loading shader file : '%s'", it.c_str());
		string & buffer = buffers[i++];
		if (!MergeFile(buffer, it))
		{
			PRINT_ERROR("Error loading shader file : '%s'!", it.c_str());
			return;
		}
	}

	LoadFromString(whichShader, buffers, includes);
}

bool Shader::MergeFile(std::string& buffer, const std::string& filename) 
{
	ifstream fp(filename, ifstream::in);
	if (fp.is_open())
	{
		string line;
		while (getline(fp, line))
		{
			buffer.append(line);
			buffer.append("\r\n");
		}

		return true;
	}
	else
	{
		return false;
	}
}

void Shader::CreateAndLinkProgram()
{
	DeleteShaderProgram();

	mProgram = glCreateProgram();
	for (auto shader : mShaders)
	{
		glAttachShader(mProgram, shader);
	}

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(mProgram);
	glGetProgramiv(mProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) 
	{
		GLint infoLogLength;

		glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(mProgram, infoLogLength, NULL, infoLog);
		infoLog[infoLogLength] = 0;
		PRINT_ERROR("Link log: %s", infoLog, 0);
		delete[] infoLog;
	}

	for (auto shader : mShaders)
	{
		glDeleteShader(shader);
	}
	mShaders.clear();
}

void Shader::Use() const
{
	glUseProgram(mProgram);
}

void Shader::UnUse() const
{
	glUseProgram(0);
}

int Shader::AddAttributes(const char * names[], int count)
{
	int offset = (int)mAttributeList.size();
	mAttributeList.reserve(mAttributeList.size() + count);
	for (auto i = 0; i < count; ++i)
	{
		mAttributeList.push_back(glGetAttribLocation(mProgram, names[i]));
		PRINT_MESSAGE("\t[%s] + attribute : name='%s' (index=%i), location=%i.", mTitle.c_str(), names[i], i + offset, mAttributeList.back());
	}

	return offset;
}

int Shader::AddUniforms(const char * names[], int count)
{
	int offset = (int)mUniformLocationList.size();
	mUniformLocationList.reserve(mUniformLocationList.size() + count);
	for (auto i = 0; i < count; ++i)
	{
		mUniformLocationList.push_back(glGetUniformLocation(mProgram, names[i]));
		PRINT_MESSAGE("\t[%s] + uniform : name='%s' (index=%i), location=%i.", mTitle.c_str(), names[i], offset + i, mUniformLocationList.back());
	}

	return offset;
}

GLint Shader::GetAttribute(int index) const
{
	assert(index < (int)mAttributeList.size());
	return index < (int)mAttributeList.size() ? mAttributeList[index] : -1;
}

GLint Shader::GetUniform(int index) const
{
	assert(index < (int)mUniformLocationList.size());
	return index < (int)mUniformLocationList.size() ? mUniformLocationList[index] : -1;
}

void Shader::SetupFrameDataBlockBinding() const
{
	GLuint blockIndex = glGetUniformBlockIndex(mProgram, "FrameData");
	glUniformBlockBinding(mProgram, blockIndex, Engine::FrameDataBuffer_BindingIndex);
	GL_CHECK_ERRORS;
}

void Shader::GenerateTexGetFunction(std::string & generatedSource, int texSamplerCount, const char * functionName, const char * samplerUniformVarName)
{
	const int tmpBufferCount = 200;
	char tmpBuffer[tmpBufferCount];

	sprintf_s(tmpBuffer, tmpBufferCount, "vec4 %s(uint samplerIndex, vec2 texUV, uint layerIndex)\r\n{\r\n", functionName);
	generatedSource.append(tmpBuffer);

	for (int itex = 0; itex < texSamplerCount; ++itex)
	{
		sprintf_s(tmpBuffer, tmpBufferCount, "%sif(samplerIndex == %i) { return texture(%s[int(%i)], vec3(texUV, layerIndex)); }\r\n", itex == 0 ? "\t" : "\telse ", itex, samplerUniformVarName, itex);
		generatedSource.append(tmpBuffer);
	}
	generatedSource.append("\treturn vec4(0);\r\n}\n");
}

int Shader::AddPerlinNoiseUniforms()
{
	const char * perlinNoiseUniforms[__PerlinNoise_uniforms_count__] =
	{
		"u_PerlinNoisePermSampler",
		"u_PerlinNoisePerm2DSampler",
		"u_PerlinNoiseGradSampler",
		"u_PerlinNoisePermGradSampler",
		"u_PerlinNoisePermGrad4DSampler",
		"u_PerlinNoiseGrad4DSampler"
	};

	return AddUniforms(perlinNoiseUniforms, __PerlinNoise_uniforms_count__);
}

} // namespace CoreFx