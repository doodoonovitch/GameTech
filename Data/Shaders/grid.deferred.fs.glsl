layout(location = 0) out vec3 outNormal;
layout(location = 1) out uvec4 outAlbedoAndStatus;
layout(location = 2) out vec4 outSpecularAndRoughness;
layout(location = 3) out vec3 outEmissive;

in VS_OUT
{
	vec2 TexUV;
} fs_in;


void main(void)
{
	outNormal = vec3(0);
	outAlbedoAndStatus = uvec3(fs_in.TexUV.xy * 255, 0, GRID_RENDERER_ID);
}
