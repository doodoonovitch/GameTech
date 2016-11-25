layout (std140, shared) uniform FrameData
{
	mat4 u_ProjMatrix;
	mat4 u_InvProjMatrix;

	mat4 u_OrthoProjMatrix;
		
	mat4 u_ViewMatrix;

	mat4 u_ViewProjMatrix;

	DualQuat u_ViewDQ;

	vec4 u_ViewPosition;

	vec4 u_AmbientLight;

	vec4 u_WireFrameDrawColor;
	vec4 u_VertexNormalColor;

	vec4 u_BufferViewportSize;
	vec4 u_NearFarFovYAspect;
	vec4 u_LeftRightTopBottom;
	
	vec2 u_TimeDeltaTime;

	float u_NormalMagnitude;
	
	float u_Exposure;
	float u_InvGamma;

	int u_PointLightCount; 
	int u_SpotLightCount;
	int u_DirectionalLightCount;
};