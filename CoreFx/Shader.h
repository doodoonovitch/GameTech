#pragma once
#ifndef COREFX_SHADER_H
#define COREFX_SHADER_H


namespace CoreFx
{



class Shader
{
public:

	enum
	{
		VERTEX_ATTRIBUTE			= 0,
		UV_ATTRIBUTE				= 1,
		NORMAL_ATTRIBUTE			= 2,
		TANGENT_ATTRIBUTE			= 3,
		BINORMAL_ATTRIBUTE			= 4,
		MATERIALID_ATTRIBUTE		= 5,

		USER_FIRST_ATTRIBUTE,
	};

	enum
	{
		VIEW_MATRIX_UNIFORM			= 0,
		PROJ_MATRIX_UNIFORM			= 1,

		USER_FIRST_UNIFORM,
	};


public:
	Shader(void);
	~Shader(void);
	void LoadFromString(GLenum whichShader, const std::vector<std::string> & sources);
	void LoadFromFile(GLenum whichShader, const std::string& filename);
	void LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames);
	void CreateAndLinkProgram();
	void Use();
	void UnUse();
	void AddAttribute(const std::string& attribute);
	void AddUniform(const std::string& uniform);
	GLuint operator[](const std::string& attribute);
	GLuint operator()(const std::string& uniform);
	void DeleteShaderProgram();
	GLuint GetProgram() const { return mProgram; }

	void SetupFrameDataBlockBinding() const;

	static const char* ShaderName(GLenum shaderType);

private:
	bool MergeFile(std::string& buffer, const std::string& filename) const;

private:

	static std::string sCommonInclude;

	GLuint mProgram;
	std::vector<GLuint> mShaders;
	std::map<std::string, GLuint> mAttributeList;
	std::map<std::string, GLuint> mUniformLocationList;
};




} // namespace Core
#endif // COREFX_SHADER_H