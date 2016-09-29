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
		UV_ATTRIBUTE				= 1,
		NORMAL_ATTRIBUTE			= 2,
		TANGENT_ATTRIBUTE			= 3,
		MESHID_ATTRIBUTE			= 4,

		USER_FIRST_ATTRIBUTE,
	};

	enum
	{
		VIEW_MATRIX_UNIFORM			= 0,
		PROJ_MATRIX_UNIFORM			= 1,

		USER_FIRST_UNIFORM,
	};

	enum
	{
		u_PerlinNoisePermSampler,
		u_PerlinNoisePerm2DSampler,
		u_PerlinNoiseGradSampler,
		u_PerlinNoisePermGradSampler,
		u_PerlinNoisePermGrad4DSampler,
		u_PerlinNoiseGrad4DSampler,

		__PerlinNoise_uniforms_count__
	};

	enum class EIncludeCommon
	{
		None = 0,
		RendererShadersHeader = 1,
		ComputeShadersHeader = 2,
		Quaternions = 4,
		FrameCommon = 8,
	};

	enum class EInclude
	{
		None = 0,
		RendererShadersCommon = (GLuint)EIncludeCommon::RendererShadersHeader | (GLuint)EIncludeCommon::Quaternions | (GLuint)EIncludeCommon::FrameCommon,
		ComputeShadersCommon = (GLuint)EIncludeCommon::ComputeShadersHeader | (GLuint)EIncludeCommon::Quaternions,
	};

public:
	Shader(const char * title);
	virtual ~Shader(void);

	bool IsLoaded() const { return mProgram != 0; }

	void LoadFromString(GLenum whichShader, const std::vector<std::string> & sources, EInclude includes = EInclude::RendererShadersCommon);
	void LoadFromFile(GLenum whichShader, const std::string& filename, EInclude includes = EInclude::RendererShadersCommon);
	void LoadFromFile(GLenum whichShader, const std::vector<std::string> & filenames, EInclude includes = EInclude::RendererShadersCommon);
	void CreateAndLinkProgram();
	void DeleteShaderProgram();

	void Use() const;
	void UnUse() const;
	int AddAttributes(const char * names[], int count);
	int AddUniforms(const char * names[], int count);
	GLint GetAttribute(int index) const;
	GLint GetUniform(int index) const;
	GLint GetProgram() const { return mProgram; }

	void SetupFrameDataBlockBinding() const;

	static const char* ShaderName(GLenum shaderType);

	static bool MergeFile(std::string& buffer, const std::string& filename);

	static void GenerateTexGetFunction(std::string & generatedSource, int texSamplerCount, const char * functionName = "TexGet", const char * samplerUniformVarName = "u_textureSampler");

	int AddPerlinNoiseUniforms();


protected:

	static const char * GetCommonIncludeFilename(GLuint includeFileId);
	static bool LoadCommonInclude(std::string & buffer, EInclude includeFileId);

protected:

	//static std::string sCommonInclude;
	static std::string sRendererShadersCommon;
	static std::string sComputeShadersCommon;

	GLuint mProgram;
	std::vector<GLuint> mShaders;
	std::vector<GLint> mAttributeList;
	std::vector<GLint> mUniformLocationList;

	std::string mTitle;
};


} // namespace Core
#endif // COREFX_SHADER_H