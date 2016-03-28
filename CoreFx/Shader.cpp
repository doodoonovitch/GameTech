#include "stdafx.h"
#include "CoreFx.h"

using namespace std;

namespace CoreFx
{

	std::string Shader::sCommonInclude;

Shader::Shader(void)
	: mProgram(0)
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

void Shader::LoadFromString(GLenum whichShader, const std::vector<std::string> & sources, bool includeCommon)
{
	if (includeCommon && sCommonInclude.empty())
	{
		if (!MergeFile(sCommonInclude, "shaders/common.glsl"))
		{
			cerr << "Error loading common include file : 'shaders/common.inc' !" << endl;
			return;
		}
	}

	GLuint shader = glCreateShader(whichShader);


	std::vector<const char *> tmp(sources.size() + (includeCommon ? 1 : 0));

	int i = 0;
	if (includeCommon)
	{
		tmp[i++] = sCommonInclude.c_str();
	}

	for (auto it = sources.begin(); it != sources.end(); ++it)
	{
		tmp[i++] = it->c_str();
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
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);
		cerr << endl << ShaderName(whichShader) << " Compile log : " << infoLog << endl;
		delete[] infoLog;
	}
	mShaders.push_back(shader);
}

void Shader::LoadFromFile(GLenum whichShader, const std::string& filename, bool includeCommon)
{
	std::vector<string> buffers(1);
	string & buffer = buffers.back();

	cout << "Loading shader file : " << filename.c_str() << endl;
	if (MergeFile(buffer, filename))
	{
		LoadFromString(whichShader, buffers, includeCommon);
	}
	else
	{
		cerr << "Error loading shader : '" << filename << "'!" << endl;
	}
}

void Shader::LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames, bool includeCommon)
{
	std::vector<string> buffers(filenames.size());

	int i = 0;
	for (auto it : filenames)
	{		
		cout << "Loading shader file : " << it.c_str() << endl;
		string & buffer = buffers[i++];
		if (!MergeFile(buffer, it))
		{
			cerr << "Error loading shader file : '" << it << "'!" << endl;
			return;
		}
	}

	LoadFromString(whichShader, buffers, includeCommon);
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
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(mProgram, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
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

void Shader::AddAttribute(const std::string& attribute)
{
	mAttributeList[attribute] = glGetAttribLocation(mProgram, attribute.c_str());
	GL_CHECK_ERRORS;
}

void Shader::AddUniform(const std::string& uniform)
{
	mUniformLocationList[uniform] = glGetUniformLocation(mProgram, uniform.c_str());
	GL_CHECK_ERRORS;
}

GLint Shader::GetAttribute(const std::string& attribute) const
{
	auto it = mAttributeList.find(attribute);
	if (it == mAttributeList.end())
		return -1;
	else
		return it->second;
}

GLint Shader::GetUniform(const std::string& uniform) const
{
	auto it = mUniformLocationList.find(uniform);
	if (it == mUniformLocationList.end())
		return -1;
	else
		return it->second;
}

void Shader::SetupFrameDataBlockBinding() const
{
	GLuint blockIndex = glGetUniformBlockIndex(mProgram, "FrameData");
	glUniformBlockBinding(mProgram, blockIndex, Engine::FrameDataBuffer_BindingIndex);
	GL_CHECK_ERRORS;
}

void Shader::GenerateTexGetFunction(std::string & generatedSource, int texSamplerCount, const char * samplerUniformVarName)
{
	generatedSource = "vec4 TexGet(int samplerIndex, vec3 p)\r\n{\r\n";
	const int tmpBufferCount = 200;
	char tmpBuffer[tmpBufferCount];
	for (int itex = 0; itex < texSamplerCount; ++itex)
	{
		sprintf_s(tmpBuffer, tmpBufferCount, "%sif(samplerIndex == %i) { return texture(%s[%i], p); }\r\n", itex == 0 ? "\t" : "\telse ", itex, samplerUniformVarName, itex);
		generatedSource.append(tmpBuffer);
	}
	generatedSource.append("\treturn vec4(0);\r\n}");
}

} // namespace CoreFx