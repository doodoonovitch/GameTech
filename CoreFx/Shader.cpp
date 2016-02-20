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

void Shader::LoadFromString(GLenum whichShader, const std::vector<std::string> & sources)
{
	if (sCommonInclude.empty())
	{
		if (!MergeFile(sCommonInclude, "shaders/common.inc"))
		{
			cerr << "Error loading common include file : 'shaders/common.inc' !" << endl;
			return;
		}
	}

	GLuint shader = glCreateShader(whichShader);

	std::vector<const char *> tmp(sources.size() + 1);

	tmp[0] = sCommonInclude.c_str();
	int i = 1;
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

void Shader::LoadFromFile(GLenum whichShader, const std::string& filename)
{
	std::vector<string> buffers(1);
	string & buffer = buffers.back();

	if (MergeFile(buffer, filename))
	{
		LoadFromString(whichShader, buffers);
	}
	else
	{
		cerr << "Error loading shader : '" << filename << "'!" << endl;
	}
}

void Shader::LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames)
{
	std::vector<string> buffers(filenames.size());

	int i = 0;
	for (auto it : filenames)
	{		
		string & buffer = buffers[i++];
		if (!MergeFile(buffer, it))
		{
			cerr << "Error loading shader file : '" << it << "'!" << endl;
			return;
		}
	}

	LoadFromString(whichShader, buffers);
}

bool Shader::MergeFile(std::string& buffer, const std::string& filename) const
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
	if (status == GL_FALSE) {
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

	//glDeleteShader(mShaders[VERTEX_SHADER]);
	//glDeleteShader(mShaders[FRAGMENT_SHADER]);
	//glDeleteShader(mShaders[GEOMETRY_SHADER]);
}

void Shader::Use()
{
	glUseProgram(mProgram);
}

void Shader::UnUse()
{
	glUseProgram(0);
}

void Shader::AddAttribute(const std::string& attribute)
{
	mAttributeList[attribute] = glGetAttribLocation(mProgram, attribute.c_str());
}

void Shader::AddUniform(const std::string& uniform)
{
	mUniformLocationList[uniform] = glGetUniformLocation(mProgram, uniform.c_str());
}

GLuint Shader::operator[](const std::string& attribute)
{
	return mAttributeList[attribute];
}

GLuint Shader::operator()(const std::string& uniform)
{
	return mUniformLocationList[uniform];
}

void Shader::SetupFrameDataBlockBinding() const
{
	GLuint blockIndex = glGetUniformBlockIndex(mProgram, "FrameData");
	glUniformBlockBinding(mProgram, blockIndex, Engine::FrameDataBuffer_BindingIndex);
	GL_CHECK_ERRORS;
}

} // namespace CoreFx