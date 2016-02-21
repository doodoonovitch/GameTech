layout (location=0) out vec4 vFragColor;	//fragment shader output

//uniforms
//uniform mat4 mMV;				//modelview matrix
//uniform sampler2D textureMap;	//texture for the current mesh/submesh
//uniform float hasTexture;		//if we want to use a default colour
//uniform vec3 vLightPosition;		//light position in object space

//inputs from the vertex shader
//smooth in vec3 vEyeSpaceNormal;    //eye space normal from the vertex shader   
smooth in vec3 vEyeSpacePosition;  //eye space position from the vertex shader
smooth in vec2 vUVout;			   //texture coordinates from the vertex shader

//shader constants
const float k0 = 1.0;	//constant attenuation
const float k1 = 0.0;	//linear attenuation
const float k2 = 0.0;	//quadratic attenuation

void main()
{ 
	//get the eye space light position
	//vec4 vEyeSpaceLightPosition = (mMV * vec4(vLightPosition,1));

	//get the light vector
	//vec3 L = (vEyeSpaceLightPosition.xyz - vEyeSpacePosition);

	//get the distance of light
	//float d = length(L);

	//normalize the light vector
	//L = normalize(L);

	//get the diffuse component
	//float diffuse = max(0, dot(vEyeSpaceNormal, L));

	//apply attenuation
	//float attenuationAmount = 1.0 / (k0 + (k1 * d) + (k2 * d * d));
	//diffuse *= attenuationAmount;

	//return final output colour
	//vFragColor = diffuse * mix(vec4(1), texture(textureMap, vUVout), hasTexture);
	vFragColor = vec4(1, 0.5, 0.5, 0);
}