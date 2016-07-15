#include "stdafx.h"
#include "CoreFx.h"


namespace CoreFx
{
	namespace Renderers
	{



SkydomeRenderer::SkydomeRenderer(const int rings, const int segments)
	: RendererHelper<2>(0, "SkydomeRenderer", "SkydomeWireFrameRenderer", Renderer::Forward_Pass)
{
	PRINT_MESSAGE("Initialize SkydomeRenderer....");

	mCosThetaSun = cos(-1.0f);
	mThetaSun = glm::vec2(-1.0f, mCosThetaSun * mCosThetaSun);
	mPhiSun = -1.0f;
	mSunSolidAngle = 0.25f * glm::pi<float>() * 1.39f * 1.39f / (150.0f * 150.0f);

	// Init neta table

	for (int i = 0; i < 1801; ++i)
		mNetaTable[i] = glm::vec3(static_cast< float >(s_neta[i * 3 + 0]), static_cast< float >(s_neta[i * 3 + 1]), static_cast< float >(s_neta[i * 3 + 2]));

	// Init aerial perspective data 

	SetTurbidity(6.0f);
	SetSun(0.0f, 0.0f);

	mConstants = glm::vec4(static_cast< float >(c_thetaBins), static_cast< float >(c_phiBins), 0.1f, 0.007f); // thetaBins, phiBins, exposure, inscatter scale


	// -----------------------------------------------------------------------
	const char * uniformNames[__uniforms_count__] =
	{
		"u_sunDirection",
		"u_thetaSun",
		"u_zenithDirection",
		"u_zenithColor",
		"u_A",
		"u_B",
		"u_C",
		"u_D",
		"u_E",
		"u_colorConvMat",
	};

	const glm::vec3 zenith(0.f, 1.f, 0.f);
	//setup shader
	mShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skydome.vs.glsl");
	mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skydome.forward.fs.glsl");
	//mShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.deferred.fs.glsl");
	mShader.CreateAndLinkProgram();
	mShader.Use();
		mShader.AddUniforms(uniformNames, __uniforms_count__);

		glUniformMatrix3fv(mShader.GetUniform(u_colorConvMat), 1, true, s_CM);
		glUniform3fv(mShader.GetUniform(u_zenithDirection), 1, glm::value_ptr(zenith));

		mShader.SetupFrameDataBlockBinding();
	mShader.UnUse();

	GL_CHECK_ERRORS;

	//setup shader
	mWireFrameShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/skybox.vs.glsl");
	mWireFrameShader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/skybox.wireframe.fs.glsl");
	mWireFrameShader.CreateAndLinkProgram();
	mWireFrameShader.Use();
	mWireFrameShader.SetupFrameDataBlockBinding();
	mWireFrameShader.UnUse();

	GL_CHECK_ERRORS;

	std::vector<glm::vec3> vertices;
	std::vector<GLushort> indices;

	CreateGeometry(vertices, indices, 5160.0f, 1000.0f, rings, segments);
	mIndiceCount = (GLsizei)indices.size();

	//setup vao and vbo stuff
	CreateBuffers();

	glBindVertexArray(mVaoID);

		glBindBuffer(GL_ARRAY_BUFFER, mVboIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

		glEnableVertexAttribArray(Shader::POSITION_ATTRIBUTE);
		glVertexAttribPointer(Shader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		GL_CHECK_ERRORS;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIDs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * mIndiceCount, indices.data(), GL_STATIC_DRAW);
		GL_CHECK_ERRORS;

	glBindVertexArray(0);

	GL_CHECK_ERRORS;

	PRINT_MESSAGE("... SkydomeRenderer initialized!\n");
}

SkydomeRenderer::~SkydomeRenderer()
{
}

void SkydomeRenderer::CreateGeometry(std::vector<glm::vec3> & vertices, std::vector<GLushort> & indices, GLfloat radius_, const GLfloat height_, const int rings_, const int segments_)
{
	// Destroy current instance

	vertices.clear();
	indices.clear();

	// Clamp arguments
	const GLfloat Epsilon = glm::epsilon<float>();
	GLfloat radius = max(radius_, Epsilon);
	GLfloat height = max(height_, Epsilon);

	int rings = max(rings_, 1) + 1;
	int segments = max(segments_, 4) + 1;

	// Init vertex data container

	vertices.reserve(rings * (segments + 1));

	// Fill vertex data container

	for (int y = 0; y <= segments; ++y)
	{
		GLfloat theta = static_cast<GLfloat>(y) / segments * glm::two_pi<GLfloat>();

		for (int x = rings - 1; x >= 0; --x)
		{
			GLfloat phi = static_cast< GLfloat >(x) / rings * glm::half_pi<GLfloat>();

			vertices.push_back(glm::vec3(static_cast< GLfloat >(sin(phi) * cos(theta) * radius)
				, static_cast< GLfloat >(cos(phi) * height)
				, static_cast< GLfloat >(sin(phi) * sin(theta) * radius)));
		}
	}

	// Init index container

	indices.reserve(segments * rings * 2);

	// Fill index container

	bool leftToRight = false;

	for (int y = 0; y < segments; ++y)
	{
		if (true == leftToRight)
		{
			for (int x = 0; x < rings; ++x)
			{
				indices.push_back((GLushort)(y      * rings + x));
				indices.push_back((GLushort)((y + 1) * rings + x));
			}
		}
		else
		{
			for (int x = rings - 1; x >= 0; --x)
			{
				indices.push_back((GLushort)((y + 1) * rings + x));
				indices.push_back((GLushort)(y      * rings + x));
			}
		}

		leftToRight = !leftToRight;
	}
}

void SkydomeRenderer::SetSunDirection(const glm::vec3 sunDirection)
{ 
	mSunDirection = sunDirection; 
	SetSun(acos(mSunDirection.y), atan2(mSunDirection.z, mSunDirection.x));
}


void SkydomeRenderer::UpdatePreethamScatterParams()
{
	glUniform3fv(mShader.GetUniform(u_sunDirection), 1, glm::value_ptr(mSunDirection));
	glUniform2fv(mShader.GetUniform(u_thetaSun), 1, glm::value_ptr(mThetaSun));
	glUniform3fv(mShader.GetUniform(u_zenithColor), 1, glm::value_ptr(mxyYZenith));
	glUniform3fv(mShader.GetUniform(u_A), 1, glm::value_ptr(mAxyY));
	glUniform3fv(mShader.GetUniform(u_B), 1, glm::value_ptr(mBxyY));
	glUniform3fv(mShader.GetUniform(u_C), 1, glm::value_ptr(mCxyY));
	glUniform3fv(mShader.GetUniform(u_D), 1, glm::value_ptr(mDxyY));
	glUniform3fv(mShader.GetUniform(u_E), 1, glm::value_ptr(mExyY));
}

void SkydomeRenderer::Render()
{
	mShader.Use();
	UpdatePreethamScatterParams();
		glBindVertexArray(mVaoID);
			glDrawElements(GL_TRIANGLE_STRIP, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);
		glBindVertexArray(0);
	mShader.UnUse();
}
 
void SkydomeRenderer::RenderWireFrame()
{
	mWireFrameShader.Use();
		glBindVertexArray(mVaoID);
			glDrawElements(GL_TRIANGLE_STRIP, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);
		glBindVertexArray(0);
	mWireFrameShader.UnUse();
}


//-----------------------------------------------------------------------------
//  angleBetween
//-----------------------------------------------------------------------------
float SkydomeRenderer::AngleBetween(const float& thetav, const float& phiv, const float& theta, const float& phi)
{
	float cospsi = glm::sin(thetav) * glm::sin(theta) * glm::cos(phi - phiv) + glm::cos(thetav) * glm::cos(theta);

	if (cospsi > 1.0f)
		return 0.0;

	if (cospsi < -1.0f)
		return glm::pi<float>();

	return glm::acos(cospsi);
}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Static initialization
//*********************************************************************************************************************
//*********************************************************************************************************************

const float SkydomeRenderer::s_xDC[5][2] =
{
	{ -0.0193f, -0.2592f },
	{ -0.0665f,  0.0008f },
	{ -0.0004f,  0.2125f },
	{ -0.0641f, -0.8989f },
	{ -0.0033f,  0.0452f }
};

const float SkydomeRenderer::s_yDC[5][2] =
{
	{ -0.0167f, -0.2608f },
	{ -0.0950f,  0.0092f },
	{ -0.0079f,  0.2102f },
	{ -0.0441f, -1.6537f },
	{ -0.0109f,  0.0529f }
};

const float SkydomeRenderer::s_YDC[5][2] =
{
	{ 0.1787f, -1.4630f },
	{ -0.3554f,  0.4275f },
	{ -0.0227f,  5.3251f },
	{ 0.1206f, -2.5771f },
	{ -0.0670f,  0.3703f }
};

const float SkydomeRenderer::s_xZC[3][4] =
{
	{ 0.00166f, -0.00375f,  0.00209f, 0.00000f },
	{ -0.02903f,  0.06377f, -0.03203f, 0.00394f },
	{ 0.11693f, -0.21196f,  0.06052f, 0.25886f }
};

const float SkydomeRenderer::s_yZC[3][4] =
{
	{ 0.00275f, -0.00610f,  0.00317f, 0.00000f },
	{ -0.04214f,  0.08970f, -0.04153f, 0.00516f },
	{ 0.15346f, -0.26756f,  0.06670f, 0.26688f }
};

const float SkydomeRenderer::s_CM[9] =
{
	3.240479f, -1.537150f, -0.498535f,
	-0.969256f,  1.875992f,  0.041556f,
	0.055648f, -0.204043f,  1.057311f
};

const float SkydomeRenderer::s_CIE_300_800[107][3] =
{
#include "PreethamScatter/ciexyz31_300_800.table"
};

const float SkydomeRenderer::s_CIE_350_800[97][3] =
{
#include "PreethamScatter/ciexyz31_350_800.table"
};

const float SkydomeRenderer::s_neta[] =
{
#include "PreethamScatter/neta.table"
};

const float SkydomeRenderer::s_K[] =
{
#include "PreethamScatter/K.table"
};

const float SkydomeRenderer::s_N21[] =
{
#include "PreethamScatter/N21.table"
};

const float SkydomeRenderer::s_Ko[] =
{
#include "PreethamScatter/Ko.table"
};

const float SkydomeRenderer::s_Kg[] =
{
#include "PreethamScatter/Kg.table"
};

const float SkydomeRenderer::s_Kwa[] =
{
#include "PreethamScatter/Kwa.table"
};

const float SkydomeRenderer::s_sol[] =
{
#include "PreethamScatter/Sol.table"
};

const float SkydomeRenderer::s_V = 4.0f;
const float SkydomeRenderer::s_anisCor = 1.06f;
const float SkydomeRenderer::s_N = 2.545e25f;
const float SkydomeRenderer::s_alpha1 = 0.83331810215394e-3f;
const float SkydomeRenderer::s_alpha2 = 0.11360016092149e-3f;


//-----------------------------------------------------------------------------
//  setTurbidity
//-----------------------------------------------------------------------------
void SkydomeRenderer::SetTurbidity(const float turbidity)
{
	// Calculate Perez function coefficients

	mTurbitity = turbidity;

	mAxyY.x = s_xDC[0][0] * mTurbitity + s_xDC[0][1];
	mBxyY.x = s_xDC[1][0] * mTurbitity + s_xDC[1][1];
	mCxyY.x = s_xDC[2][0] * mTurbitity + s_xDC[2][1];
	mDxyY.x = s_xDC[3][0] * mTurbitity + s_xDC[3][1];
	mExyY.x = s_xDC[4][0] * mTurbitity + s_xDC[4][1];

	mAxyY.y = s_yDC[0][0] * mTurbitity + s_yDC[0][1];
	mBxyY.y = s_yDC[1][0] * mTurbitity + s_yDC[1][1];
	mCxyY.y = s_yDC[2][0] * mTurbitity + s_yDC[2][1];
	mDxyY.y = s_yDC[3][0] * mTurbitity + s_yDC[3][1];
	mExyY.y = s_yDC[4][0] * mTurbitity + s_yDC[4][1];

	mAxyY.z = s_YDC[0][0] * mTurbitity + s_YDC[0][1];
	mBxyY.z = s_YDC[1][0] * mTurbitity + s_YDC[1][1];
	mCxyY.z = s_YDC[2][0] * mTurbitity + s_YDC[2][1];
	mDxyY.z = s_YDC[3][0] * mTurbitity + s_YDC[3][1];
	mExyY.z = s_YDC[4][0] * mTurbitity + s_YDC[4][1];

	// Calculate Mie and Rayleigh scattering coefficients

	float b_m[101], b_p[101];
	float b_m_ang_prefix[101];
	float b_p_ang_prefix[101];

	float c = (0.06544204545455f * mTurbitity - 0.06509886363636f) * 1e-15f;

	mBetaM = glm::vec3(0.0f);
	mBetaP = glm::vec3(0.0f);
	mBetaMAng = glm::vec3(0.0f);
	mBetaPAng = glm::vec3(0.0f);

	float lambda;
	int i;

	for (lambda = 300.0f, i = 0; lambda <= 800.0f; lambda += 5.0f, ++i)
	{
		float lambdasi = lambda * 1e-9f;  // Converstion to SI units
		float Nlambda4 = s_N * lambdasi * lambdasi * lambdasi * lambdasi;

		// Rayleigh total scattering coefficient

		float n2_1 = s_N21[i];

		b_m[i] = 8.0f * glm::pi<float>() * glm::pi<float>() * glm::pi<float>() * n2_1 * s_anisCor / (3.0f * Nlambda4);

		// Mie total scattering coefficient 

		float K = s_K[i];

		b_p[i] = 0.434f * glm::pi<float>() * c * pow(2.0f * glm::pi<float>() / lambdasi, s_V - 2.0f) * K * pow(0.01f, s_V - 3.0f);

		// Rayleigh angular scattering coefficient 
		// Needs to be multiplied by (1+0.9324cos^2(theta)) to get exact angular scattering constant 

		b_m_ang_prefix[i] = 2.0f * glm::pi<float>() * glm::pi<float>() * n2_1 * s_anisCor * 0.7629f / (3.0f * Nlambda4);

		// Mie angular scattering coefficient 
		// Needs to be multiplied by neta(theta,lambda) to get exact angular scattering constant term 

		b_p_ang_prefix[i] = 0.217f * c * pow(2.0f * glm::pi<float>() / lambdasi, s_V - 2.0f) * pow(0.01f, s_V - 3.0f);

		// Convert spectral curves to CIE XYZ

		mBetaM[0] += static_cast< float >(b_m[i] * s_CIE_300_800[i][0]);
		mBetaM[1] += static_cast< float >(b_m[i] * s_CIE_300_800[i][1]);
		mBetaM[2] += static_cast< float >(b_m[i] * s_CIE_300_800[i][2]);

		mBetaP[0] += static_cast< float >(b_p[i] * s_CIE_300_800[i][0]);
		mBetaP[1] += static_cast< float >(b_p[i] * s_CIE_300_800[i][1]);
		mBetaP[2] += static_cast< float >(b_p[i] * s_CIE_300_800[i][2]);

		mBetaMAng[0] += static_cast< float >(b_m_ang_prefix[i] * s_CIE_300_800[i][0]);
		mBetaMAng[1] += static_cast< float >(b_m_ang_prefix[i] * s_CIE_300_800[i][1]);
		mBetaMAng[2] += static_cast< float >(b_m_ang_prefix[i] * s_CIE_300_800[i][2]);

		mBetaPAng[0] += static_cast< float >(b_p_ang_prefix[i] * s_CIE_300_800[i][0]);
		mBetaPAng[1] += static_cast< float >(b_p_ang_prefix[i] * s_CIE_300_800[i][1]);
		mBetaPAng[2] += static_cast< float >(b_p_ang_prefix[i] * s_CIE_300_800[i][2]);
	}

	ComputeAttenuatedSunlight();
}

//-----------------------------------------------------------------------------
//  setScaleFactors
//-----------------------------------------------------------------------------
void SkydomeRenderer::SetScaleFactors(const float exposure, const float scaleInscatter)
{
	mConstants = glm::vec4(static_cast< float >(c_thetaBins), static_cast< float >(c_phiBins), exposure, scaleInscatter);
}





//-----------------------------------------------------------------------------
//  computeChromaticity
//-----------------------------------------------------------------------------
const float SkydomeRenderer::ComputeChromaticity(const float m[][4]) const
{
	float T2 = mTurbitity * mTurbitity;
	float t2 = mThetaSun.x * mThetaSun.x;
	float t3 = t2 * mThetaSun.x;

	return  (T2 * m[0][0] + mTurbitity * m[1][0] + m[2][0]) * t3 +
		(T2 * m[0][1] + mTurbitity * m[1][1] + m[2][1]) * t2 +
		(T2 * m[0][2] + mTurbitity * m[1][2] + m[2][2]) * mThetaSun.x +
		(T2 * m[0][3] + mTurbitity * m[1][3] + m[2][3]);
}

//-----------------------------------------------------------------------------
//  perezFunction
//-----------------------------------------------------------------------------
const float SkydomeRenderer::PerezFunction(const float a_A, const float a_B, const float a_C, const float a_D, const float a_E, const float a_theta, const float a_gamma) const
{
	float e0 = a_B / cos(a_theta);
	float e1 = a_D * a_gamma;
	float e2 = cos(a_gamma);
	e2 *= e2;

	float f1 = (1 + a_A * exp(e0)) * (1 + a_C * exp(e1) + a_E * e2);

	e0 = a_B;
	e1 = a_D * mThetaSun.x;
	e2 = mCosThetaSun;
	e2 *= e2;

	float f2 = (1 + a_A * exp(e0)) * (1 + a_C * exp(e1) + a_E * e2);

	return f1 / f2;
}

//-----------------------------------------------------------------------------
//  computeAttenuatedSunlight
//-----------------------------------------------------------------------------
void SkydomeRenderer::ComputeAttenuatedSunlight()
{
	float data[91];  // (800 - 350) / 5  + 1
	float beta = 0.04608365822050f * mTurbitity - 0.04586025928522f;

	float m = 1.0f / (mCosThetaSun + 0.15f * pow(93.885f - glm::degrees(mThetaSun.x), -1.253f));  // Relative Optical Mass

	mSunSpectralRad = glm::vec3(0.0f);

	int i;
	float lambda;

	for (i = 0, lambda = 350.f; i < 91; ++i, lambda += 5.0f)
	{
		// Rayleigh Scattering

		float tauR = exp(-m * 0.008735f * pow(lambda / 1000.0f, -4.08f));

		// Aerosal (water + dust) attenuation
		// beta - amount of aerosols present
		// alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

		const float alpha = 1.3f;

		float tauA = exp(-m * beta * pow(lambda / 1000.0f, -alpha));  // lambda should be in um

																	  // Attenuation due to ozone absorption
																	  // lOzone - amount of ozone in cm(NTP)

		const float lOzone = 0.35f;

		float tauO = exp(-m * s_Ko[i] * lOzone);

		// Attenuation due to mixed gases absorption

		float tauG = exp(-1.41f * s_Kg[i] * m / pow(1.0f + 118.93f * s_Kg[i] * m, 0.45f));

		// Attenuation due to water vapor absorbtion
		// w - precipitable water vapor in centimeters (standard = 2)

		const float w = 2.0f;

		float tauWA = exp(-0.2385f * s_Kwa[i] * w * m / pow(1.0f + 20.07f * s_Kwa[i] * w * m, 0.45f));

		data[i] = 100.0f * s_sol[i] * tauR * tauA * tauO * tauG * tauWA;  // 100 comes from solCurve being in wrong units. 

																		 // Convert spectral curve to CIE XYZ

		mSunSpectralRad[0] += static_cast< float >(data[i] * s_CIE_350_800[i][0]);
		mSunSpectralRad[1] += static_cast< float >(data[i] * s_CIE_350_800[i][1]);
		mSunSpectralRad[2] += static_cast< float >(data[i] * s_CIE_350_800[i][2]);
	}
}

//-----------------------------------------------------------------------------
//  getSkySpectralRadiance
//-----------------------------------------------------------------------------
void SkydomeRenderer::GetSkySpectralRadiance(glm::vec3 & a_color, const float a_theta, const float a_phi) const
{
	float gamma = AngleBetween(a_theta, a_phi, mThetaSun.x, mPhiSun);

	float x = mxyYZenith.x * PerezFunction(mAxyY.x, mBxyY.x, mCxyY.x, mDxyY.x, mExyY.x, a_theta, gamma);
	float y = mxyYZenith.y * PerezFunction(mAxyY.y, mBxyY.y, mCxyY.y, mDxyY.y, mExyY.y, a_theta, gamma);
	float Y = mxyYZenith.z * PerezFunction(mAxyY.z, mBxyY.z, mCxyY.z, mDxyY.z, mExyY.z, a_theta, gamma);

	//    Y = 1.0 - exp( -m_constants.GetZ() * Y );

	float X = (x / y) * Y;
	float Z = ((1.0f - x - y) / y) * Y;

	a_color = glm::vec3(static_cast< float >(X), static_cast< float >(Y), static_cast< float >(Z));
}

//-----------------------------------------------------------------------------
//  setSun
//-----------------------------------------------------------------------------
void SkydomeRenderer::SetSun(const float a_theta, const float a_phi)
{
	// Check need for update

	bool bInitA0 = false;

	if (a_theta != mThetaSun.x)
		bInitA0 = true;

	if (a_phi != mPhiSun)
		bInitA0 = true;

	mThetaSun.x = a_theta;
	mCosThetaSun = cos(mThetaSun.x);
	mThetaSun.y = mCosThetaSun * mCosThetaSun;
	mPhiSun = a_phi;

	// Calculate sun luminance and chromaticity values

	float dChi = (4.0f / 9.0f - mTurbitity / 120.0f) * (glm::pi<float>() - 2.0f * mThetaSun.x);

	mxyYZenith.z = (4.0453f * mTurbitity - 4.9710f) * tan(dChi) - 0.2155f * mTurbitity + 2.4192f;
	//  m_YZenith *= 1000.0; // conversion from kcd/m^2 to cd/m^2

	if (mxyYZenith.z < 0.0f)
		mxyYZenith.z = -mxyYZenith.z;

	mxyYZenith.x = ComputeChromaticity(s_xZC);
	mxyYZenith.y = ComputeChromaticity(s_yZC);

	// Calculate light scattered into view direction

	if (true == bInitA0)
	{
		ComputeAttenuatedSunlight();

		int i, j;
		float theta, phi;

		float delTheta = glm::pi<float>() / c_thetaBins;
		float delPhi = 2.0f * glm::pi<float>() / c_phiBins;

		for (i = 0, theta = 0.f; theta <= glm::pi<float>(); theta += delTheta, ++i)
			for (j = 0, phi = 0.f; phi <= 2.0f * glm::pi<float>(); phi += delPhi, ++j)
				CalculateS0(theta, phi, mS0_Mie[i * (c_thetaBins + 1) + j], mS0_Ray[i * (c_thetaBins + 1) + j]);
	}
}

//-----------------------------------------------------------------------------
//  getNeta
//-----------------------------------------------------------------------------
void SkydomeRenderer::GetNeta(const float a_theta, glm::vec3 & a_neta) const
{
	float theta = glm::degrees(a_theta) * 10.0f;
	float u = theta - (int)theta;

	if (theta < 0.0 || theta > 1801.0)
	{
		a_neta = glm::vec3(0.0);
		return;
	}

	if (theta > 1800.f)
	{
		a_neta = mNetaTable[1800];
		return;
	}

	a_neta = (1.0f - (float)u) * mNetaTable[(int)theta] + (float)u * mNetaTable[(int)theta + 1];
}

//-----------------------------------------------------------------------------
//  calculateS0
//-----------------------------------------------------------------------------
void SkydomeRenderer::CalculateS0(const float a_thetav, const float a_phiv, glm::vec3 & a_S0_Mie, glm::vec3 & a_S0_Ray) const
{
	float phiDelta = glm::pi<float>() / 20.0f;
	float thetaDelta = glm::pi<float>() / 2.0f / 20.0f;
	float thetaUpper = glm::pi<float>() / 2.0f;
	float psi;

	glm::vec3 skyRad;
	glm::vec3 beta_ang_1, beta_ang_2;
	glm::vec3 neta;
	glm::vec3 skyAmb_1(0.0f, 0.0f, 0.0f);
	glm::vec3 skyAmb_2(0.0f, 0.0f, 0.0f);

	for (float theta = 0; theta < thetaUpper; theta += thetaDelta)
	{
		for (float phi = 0; phi < 2.0f * glm::pi<float>(); phi += phiDelta)
		{
			GetSkySpectralRadiance(skyRad, theta, phi);
			psi = AngleBetween(a_thetav, a_phiv, theta, phi);

			GetNeta(psi, neta);
			beta_ang_1 = mBetaPAng * neta;
			beta_ang_2 = mBetaMAng * static_cast< float >((1.0f + 0.9324f * cos(psi) * cos(psi)));

			skyAmb_1 += skyRad * beta_ang_1 * static_cast< float >(sin(theta) * thetaDelta * phiDelta);
			skyAmb_2 += skyRad * beta_ang_2 * static_cast< float >(sin(theta) * thetaDelta * phiDelta);
		}
	}

	psi = AngleBetween(a_thetav, a_phiv, mThetaSun.x, mPhiSun);

	GetNeta(psi, neta);
	beta_ang_1 = mBetaPAng * neta;
	beta_ang_2 = mBetaMAng * static_cast< float >((1.0f + 0.9324f * cos(psi) * cos(psi)));

	glm::vec3 sunAmb_1 = mSunSpectralRad * beta_ang_1 * static_cast< float >(mSunSolidAngle);
	glm::vec3 sunAmb_2 = mSunSpectralRad * beta_ang_2 * static_cast< float >(mSunSolidAngle);

	a_S0_Mie = sunAmb_1 + skyAmb_1;
	a_S0_Ray = sunAmb_2 + skyAmb_2;
}


	} // namespace Renderers
} // namespace CoreFx