#include "stdafx.h"
#include "CoreFx.h"

using namespace std;

namespace CoreFx
{


Shader::Shader(void)
	: _program(0)
	, _totalShaders(0)
{
	_shaders[VERTEX_SHADER] = 0;
	_shaders[FRAGMENT_SHADER] = 0;
	_shaders[GEOMETRY_SHADER] = 0;
	_attributeList.clear();
	_uniformLocationList.clear();
}

Shader::~Shader(void)
{
	DeleteShaderProgram();
	_attributeList.clear();
	_uniformLocationList.clear();
}

void Shader::DeleteShaderProgram()
{
	glDeleteProgram(_program);
	_program = 0;
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
	_shaders[_totalShaders++] = shader;
}

void Shader::LoadFromFile(GLenum whichShader, const std::string& filename)
{
	ifstream fp(filename, ifstream::in);
	if (fp.is_open()) 
	{
		string line, buffer;
		while (getline(fp, line)) 
		{
			buffer.append(line);
			buffer.append("\r\n");
		}
		//copy to source
		LoadFromString(whichShader, buffer);
	}
	else 
	{
		cerr << "Error loading shader: " << filename << endl;
	}
}

void Shader::CreateAndLinkProgram()
{
	DeleteShaderProgram();

	_program = glCreateProgram();
	if (_shaders[VERTEX_SHADER] != 0) 
	{
		glAttachShader(_program, _shaders[VERTEX_SHADER]);
	}
	if (_shaders[FRAGMENT_SHADER] != 0) 
	{
		glAttachShader(_program, _shaders[FRAGMENT_SHADER]);
	}
	if (_shaders[GEOMETRY_SHADER] != 0) 
	{
		glAttachShader(_program, _shaders[GEOMETRY_SHADER]);
	}

	//link and check whether the program links fine
	GLint status;
	glLinkProgram(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;

		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(_program, infoLogLength, NULL, infoLog);
		cerr << "Link log: " << infoLog << endl;
		delete[] infoLog;
	}

	glDeleteShader(_shaders[VERTEX_SHADER]);
	glDeleteShader(_shaders[FRAGMENT_SHADER]);
	glDeleteShader(_shaders[GEOMETRY_SHADER]);
}

void Shader::Use()
{
	glUseProgram(_program);
}

void Shader::UnUse()
{
	glUseProgram(0);
}

void Shader::AddAttribute(const std::string& attribute)
{
	_attributeList[attribute] = glGetAttribLocation(_program, attribute.c_str());
}

void Shader::AddUniform(const std::string& uniform)
{
	_uniformLocationList[uniform] = glGetUniformLocation(_program, uniform.c_str());
}

GLuint Shader::operator[](const std::string& attribute)
{
	return _attributeList[attribute];
}

GLuint Shader::operator()(const std::string& uniform)
{
	return _uniformLocationList[uniform];
}


} // namespace CoreFx