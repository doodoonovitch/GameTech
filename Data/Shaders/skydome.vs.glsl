layout(location = 0) in vec3 in_Position;	//vertex position

uniform vec3    u_sunDirection;
uniform vec2    u_thetaSun;
uniform vec3    u_zenithDirection;
uniform vec3    u_zenithColor;
uniform vec3    u_A;
uniform vec3    u_B;
uniform vec3    u_C;
uniform vec3    u_D;
uniform vec3    u_E;
uniform mat3	u_colorConvMat;
//uniform float   u_exposure;


out VS_OUT
{
	vec3 Color;
} vs_out;

void main()
{  
	vec3 direction	= normalize( in_Position );

	float theta     = dot( u_zenithDirection, direction );
	float gamma     = dot( u_sunDirection, direction ); 
	float cos2gamma = gamma * gamma;
	      gamma     = acos( gamma );

	//                                                                                                    
	// Compute sky chromaticity values and sky luminance                                                  
	//                                                                                                    
	                                                                                                      
	vec3 num = (1.0 + u_A * exp( u_B / theta )) * (1.0 + u_C * exp( u_D * gamma ) + u_E * cos2gamma);
	vec3 den = (1.0 + u_A * exp( u_B )) * (1.0 + u_C * exp( u_D * u_thetaSun.x ) + u_E * u_thetaSun.y);
	vec3 xyY = num / den * u_zenithColor;
	                                                 
	//                                               
	// Exposure function for luminance               
	//                                               
	                                                 
	//xyY.z = 1.0 - exp( -u_Exposure * xyY.z );
	                                                 
	//                                               
	// Convert CIE xyY to CIE XYZ                    
	//                                               
	                                                 
	vec3 XYZ;                                      
	                                                 
	XYZ.x = (xyY.x / xyY.y) * xyY.z;                 
	XYZ.y = xyY.z;                                   
	XYZ.z = ((1.0 - xyY.x - xyY.y) / xyY.y) * xyY.z; 

	//                                                
	// Convert CIE XYZ to RGB assuming D65 white point
	//                                                
	                                                  
	vs_out.Color = u_colorConvMat * XYZ;

	//                                               
	// Transform vertex                              
	//                                               
	vec4 pos = u_ProjMatrix * u_ViewMatrix * vec4(in_Position, 0);
	gl_Position = pos.xyww;
}
