layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out uvec4 outAlbedoAndStatus;
layout(location = 3) out vec4 outSpecularAndRoughness;
layout(location = 4) out vec3 outEmissive;

in VS_OUT
{
	vec4 Position;
	vec2 TexUV;
} fs_in;


void main(void)
{
	outPosition = fs_in.Position.xyz;
	outNormal = vec4(0, gl_FragCoord.z);
	outAlbedoAndStatus = uvec3(fs_in.TexUV.xy * 255, 0, GRID_RENDERER_ID);
}
