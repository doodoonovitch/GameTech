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
		POSITION_ATTRIBUTE			= 0,
		NORMAL_ATTRIBUTE			= 1,
		TANGENT_ATTRIBUTE			= 2,
		UV_ATTRIBUTE				= 3,
		MATERIALID_ATTRIBUTE		= 4,
		BINORMAL_ATTRIBUTE			= 5,

		USER_FIRST_ATTRIBUTE,
	};

	enum
	{
		VIEW_MATRIX_UNIFORM			= 0,
		PROJ_MATRIX_UNIFORM			= 1,

		USER_FIRST_UNIFORM,
	};


public:
	Shader(const char * title);
	virtual ~Shader(void);
	void LoadFromString(GLenum whichShader, const std::vector<std::string> & sources, bool includeCommon = true);
	void LoadFromFile(GLenum whichShader, const std::string& filename, bool includeCommon = true);
	void LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames, bool includeCommon = true);
	void CreateAndLinkProgram();
	void DeleteShaderProgram();

	void Use() const;
	void UnUse() const;
	void AddAttributes(const char * names[], int count);
	void AddUniforms(const char * names[], int count);
	GLint GetAttribute(int index) const;
	GLint GetUniform(int index) const;
	GLint GetProgram() const { return mProgram; }

	void SetupFrameDataBlockBinding() const;

	static const char* ShaderName(GLenum shaderType);

	static bool MergeFile(std::string& buffer, const std::string& filename);

	static void GenerateTexGetFunction(std::string & generatedSource, int texSamplerCount, const char * functionName = "TexGet", const char * samplerUniformVarName = "u_textureSampler");

protected:

	static std::string sCommonInclude;

	GLuint mProgram;
	std::vector<GLuint> mShaders;
	std::vector<GLint> mAttributeList;
	std::vector<GLint> mUniformLocationList;

	std::string mTitle;
};


} // namespace Core
#endif // COREFX_SHADER_H