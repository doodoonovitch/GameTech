#pragma once

#ifndef COREFX_RENDERERS_SKYDOMERENDERER_H
#define COREFX_RENDERERS_SKYDOMERENDERER_H


namespace CoreFx
{
	namespace Renderers
	{


class SkydomeRenderer : public RendererHelper<2>
{
public:
	SkydomeRenderer(bool useTextureCache = false, GLsizei cacheTextureSize = 512, const int rings = 128, const int segments = 128);

	virtual ~SkydomeRenderer();

	virtual void Render() override;
	virtual void RenderWireFrame() override;

	void SetSunDirection(const glm::vec3 sunDirection);
	const glm::vec3 & GetSunDirection() const { return mSunDirection; }

	bool IsCacheRebuildRequired() const { return (mCache != nullptr) ? (!mCache->mIsBuilt) : false; }
	bool UseCache() const { return mCache != nullptr; }

	CubeMapTexture const * GetCacheTexture() const { return mCache != nullptr ? mCache->mCubeMapTexture : nullptr; }

	bool RenderCache();

public:


	//
	// Accessors
	//

	//! \brief  Sets the current turbiditiy
	//! \param  [in] The turbiditiy to set
	void SetTurbidity(const float turbidity);
	//! \brief  Sets the current scale factors
	//! \param  [in] The exposure factor to set
	//! \param  [in] The inscatter scale factor to set
	void SetScaleFactors(const float exposure, const float scaleInscatter);

private:

	//
	// Skylight methods
	//

	//! \brief  Computes the chromaticity for the specified coefficients
	//! \param  [in] The chromaticity distribution coefficients to use
	//! \return The chromaticity
	const float ComputeChromaticity(const float m[][4]) const;

	//! \brief  Evaluates the Perez function
	//! \param  [in] The distribution coefficient for darkening or brightening of the horizon
	//! \param  [in] The distribution coefficient for the luminance gradient near the horizon
	//! \param  [in] The distribution coefficient for the relative intensity of the circumsolar region
	//! \param  [in] The distribution coefficient for the width of the circumsolar region
	//! \param  [in] The distribution coefficient for relative backscattered light
	//! \param  [in] The angle between sun direction and view direction 
	//! \param  [in] The angle between zenith and view direction
	//! \return The sky luminance/chromaticity distribution
	const float PerezFunction(const float A, const float B, const float C, const float D, const float E, const float theta, const float gamma) const;

	//! \brief  Computes the sun's spectral radiance for the current sun position
	void ComputeAttenuatedSunlight();

	//! \brief  Computes the sky's spectral radiance
	//! \param  [out] The spectral radiance converted to CIE XYZ
	//! \param  [in] The point of interest's angle from zenith
	//! \param  [in] The point of interest's horizontal angle
	void GetSkySpectralRadiance(glm::vec3 & color, const float theta, const float phi) const;

	//! \brief  Sets the current sun position
	//! \param  [in] The sun's angle from zenith
	//! \param  [in] The sun's horizontal angle
	void SetSun(const float theta, const float phi);

	//
	// Aerial perspective methods
	//

	//! \brief  Returns the neta value for the specified parameter
	//! \param  [in] The angle between sun direction and view direction 
	//! \param  [in] Junge's exponent
	//! \param  [out] The corresponding value for neta
	void GetNeta(const float a_theta, glm::vec3 & a_neta) const;

	//! \brief  Calculates the light scattered into the view direction 
	//! \param  [in] The view directions's angle from zenith
	//! \param  [in] The The view directions's horizontal angle
	//! \param  [out] The light scattered for Mie particles in as CIE XYZ
	//! \param  [out] The light scattered for Rayleigh particles in as CIE XYZ
	void CalculateS0(const float thetav, const float phiv, glm::vec3 & S0_Mie, glm::vec3 & S0_Ray) const;

	static float AngleBetween(const float& thetav, const float& phiv, const float& theta, const float& phi);

protected:

	void CreateGeometry(std::vector<glm::vec3> & vertices, std::vector<GLushort> & indices, GLfloat radius, const GLfloat height, const int rings, const int segments);

	void UpdatePreethamScatterParams();

	void RenderDome();

private:

	enum EUniformIndex
	{
		u_sunDirection,
		u_thetaSun,
		u_zenithDirection,
		u_zenithColor,
		u_A,
		u_B,
		u_C,
		u_D,
		u_E,
		u_colorConvMat,

		__uniforms_count__
	};

	GLsizei mIndiceCount;

	glm::vec3 mSunDirection;

	static const int c_thetaBins = 5;
	static const int c_phiBins = 5;

	//
	// Distribution tables
	//

	static const float s_xDC[5][2];  //!< Chromaticity distribution coefficients
	static const float s_yDC[5][2];  //!< Chromaticity distribution coefficients
	static const float s_YDC[5][2];  //!< Luminance distribution coefficients
	static const float s_xZC[3][4];  //!< Chromaticity distribution coefficients for zenith
	static const float s_yZC[3][4];  //!< Chromaticity distribution coefficients for zenith

									  //
									  // Color conversion tables
									  //

	static const float s_CM[9];                //!< Rec. 709 color conversion matrix
	static const float s_CIE_300_800[107][3];  //!< CIE 1931 2-deg, XYZ Color Matching Functions (300nm - 800nm)
	static const float s_CIE_350_800[97][3];   //!< CIE 1931 2-deg, XYZ Color Matching Functions (350nm - 800nm)

												//
												// Static aerial perspective tables
												//

	static const float s_neta[];
	static const float s_K[];
	static const float s_N21[];
	static const float s_Ko[];
	static const float s_Kg[];
	static const float s_Kwa[];
	static const float s_sol[];

	//
	// Aerial perspective constants
	//

	static const float s_V;
	static const float s_anisCor;
	static const float s_N;
	static const float s_alpha1;
	static const float s_alpha2;

	//
	// Dynamic aerial perspective tables
	//

	glm::vec3 mNetaTable[1801];
	glm::vec3 mS0_Mie[(c_thetaBins + 1) * (c_phiBins + 1)];  //!< Light scattered into the view direction for Mie particles
	glm::vec3 mS0_Ray[(c_thetaBins + 1) * (c_phiBins + 1)];  //!< Light scattered into the view direction for Rayleigh particles

															 //
															 // Atmosphere data
															 //
	glm::vec3 mAxyY, mBxyY, mCxyY, mDxyY, mExyY;    //!< Perez function parameter for chromaticity and luminance (xyY)
	//float mAx, mBx, mCx, mDx, mEx;    //!< Perez function parameter for chromaticity
	//float mAy, mBy, mCy, mDy, mEy;    //!< Perez function parameter for chromaticity
	//float mAY, mBY, mCY, mDY, mEY;    //!< Perez function parameter for luminance

	glm::vec3 mxyYZenith;
	//float mYZenith;   //!< Zenith luminance
	//float mxZenith;   //!< Zenith chromaticity
	//float myZenith;   //!< Zenith chromaticity

	float mTurbitity; //!< Current turbidity

						//
						// Sun data
						//

	float		mSunSolidAngle; //!< Sun's solid angle w
	glm::vec2	mThetaSun;  //!< Sun's angle from zenith
	float		mCosThetaSun;
	float		mPhiSun;        //!< Sun's horizontal angle

							//
							// Aerial perspective data
							//

	glm::vec3 mSunSpectralRad;  //!< Sun's spectral radiance
	glm::vec3 mBetaM;           //!< Rayleigh total scattering coefficient
	glm::vec3 mBetaP;           //!< Mie total scattering coefficient
	glm::vec3 mBetaMAng;        //!< Rayleigh angular scattering coefficient
	glm::vec3 mBetaPAng;        //!< Mie angular scattering coefficient
	glm::vec4 mConstants;       //!< Shader constants

	struct Cache
	{
		SkyboxRenderer * mSkyboxRenderer = nullptr;
		CubeMapTexture const * mCubeMapTexture = nullptr;
		glm::mat4 * mCubeMapProjMatrix = nullptr;
		GLuint mFBOs = 0;
		GLuint mDepthBuffer = 0;
		bool mIsBuilt = false;

		Cache();
		~Cache();
		bool Initialize(GLsizei cacheTextureSize);
	};

	Cache * mCache = nullptr;

	//CubeMapTexture const * mTextureCache = nullptr;
	//glm::mat4 * mCubeMapProjMatrix = nullptr;
	//GLuint mTextureCacheRenderFBOs = 0;
	//GLuint mTextureCacheRenderDepthBuffer = 0;
	//bool mIsCacheBuilt = false;
};



	} // namespace Renderers
} // namespace CoreFx
#endif // COREFX_RENDERERS_SKYDOMERENDERER_H