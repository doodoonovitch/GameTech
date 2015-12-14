#pragma once
#ifndef COREFX_SHADER_H
#define COREFX_SHADER_H


namespace CoreFx
{



class Shader
{
public:
	Shader(void);
	~Shader(void);
	void LoadFromString(GLenum whichShader, const std::string& source);
	void LoadFromFile(GLenum whichShader, const std::string& filename);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	void AddAttribute(const std::string& attribute);
	void AddUniform(const std::string& uniform);
	GLuint operator[](const std::string& attribute);
	GLuint operator()(const std::string& uniform);
	void DeleteShaderProgram();
private:
	enum ShaderType { VERTEX_SHADER, FRAGMENT_SHADER, GEOMETRY_SHADER };
	GLuint _program;
	int _totalShaders;
	GLuint _shaders[3];
	std::map<std::string, GLuint> _attributeList;
	std::map<std::string, GLuint> _uniformLocationList;
};




} // namespace Core
#endif // COREFX_SHADER_H