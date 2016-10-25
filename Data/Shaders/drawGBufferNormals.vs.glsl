layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

uniform ivec2 u_PatchCount;

void main()
{  
	ivec2 patchIndex = ivec2(gl_InstanceID % u_PatchCount.x, gl_InstanceID / u_PatchCount.x);

	vec2 pos = (in_Position.xy + vec2(1.0)) * 0.5;
	vec2 texUV = (pos + vec2(patchIndex.x, patchIndex.y)) / vec2(u_PatchCount.x, u_PatchCount.y);

	vec2 coef = vec2(u_ProjMatrix[0][0], u_ProjMatrix[1][1]);
	vec2 viewRay = ((texUV * 2.0) - 1.0) /  coef;


	gl_Position = vec4(texUV, viewRay);
}

