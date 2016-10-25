layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

uniform ivec2 u_PatchCount;

out VS_OUT
{
	vec2 TexUV;
	vec2 ViewRay;
} vs_out;

void main()
{  
	ivec2 patchIndex = ivec2(gl_InstanceID % u_PatchCount.x, gl_InstanceID / u_PatchCount.x);

	vec2 pos = (in_Position.xy + vec2(1.0)) * 0.5;
	vec2 texUV = (pos + vec2(patchIndex.x, patchIndex.y)) / vec2(u_PatchCount.x, u_PatchCount.y);

	vec2 coef = vec2(u_ProjMatrix[0][0], u_ProjMatrix[1][1]);
	vec2 viewRay = pos /  coef;


	gl_Position = vec4(in_Position, 1);
	vs_out.TexUV = texUV;
	vs_out.ViewRay = viewRay;
}

