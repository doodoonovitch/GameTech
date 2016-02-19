#include "stdafx.h"
#include "CoreFx.h"

using namespace std;

namespace CoreFx
{


Shader::Shader(void)
	: mProgram(0)
	, mTotalShaders(0)
{
	mShaders[VERTEX_SHADER] = 0;
	mShaders[FRAGMENT_SHADER] = 0;
	mShaders[GEOMETRY_SHADER] = 0;
	mAttributeList.clear();
	mUniformLocationList.clear();
}

Shader::~Shader(void)
{
	DeleteShaderProgram();
	mAttributeList.clear();
	mUniformLocationList.clear();
}

void Shader::DeleteShaderProgram()
{
	glDeleteProgram(mProgram);
	mProgram = 0;
}

void Shader::LoadFromString(GLenum whichShader, const std::string& source)
{
	GLuint shader = glCreateShader(whichShader);

	const char * ptmp = source.c_str();
	glShaderSource(shader, 1, &ptmp, NULL);

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
		cerr << "Compile log: " << infoLog << endl;
		delete[] infoLog;
	}
	mShaders[mTotalShaders++] = shader;
}

void Shader::LoadFromFile(GLenum whichShader, const std::string& filename, const std::vector<std::string> & includes)
{
	string buffer;

	for (auto it : includes)
	{		
		if (!MergeFile(buffer, it))
		{
			cerr << "Error loading shader include file : '" << it << "'!" << endl;
			return;
		}
	}

	if (MergeFile(buffer, filename))
	{
		LoadFromString(whichShader, buffer);
	}
	else 
	{
		cerr << "Error loading shader : '" << filename << "'!" << endl;
	}
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
	if (mShaders[VERTEX_SHADER] != 0)
	{
		glAttachShader(mProgram, mShaders[VERTEX_SHADER]);
	}
	if (mShaders[FRAGMENT_SHADER] != 0)
	{
		glAttachShader(mProgram, mShaders[FRAGMENT_SHADER]);
	}
	if (mShaders[GEOMETRY_SHADER] != 0)
	{
		glAttachShader(mProgram, mShaders[GEOMETRY_SHADER]);
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

	glDeleteShader(mShaders[VERTEX_SHADER]);
	glDeleteShader(mShaders[FRAGMENT_SHADER]);
	glDeleteShader(mShaders[GEOMETRY_SHADER]);
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


} // namespace CoreFx