layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform ivec2 u_HeightMapTextureSize;

out VS_OUT
{
	vec2 TexUV;
	int MapIndex;
} vs_out;

void main()
{  
	int patchPerTexture = u_PatchCount.x * u_PatchCount.y;
	int localIndex = gl_InstanceID % patchPerTexture;
	int layer = gl_InstanceID / patchPerTexture;

	ivec2 patchIndex = ivec2(localIndex % u_PatchCount.x, localIndex / u_PatchCount.x);

	vec2 texUV = (in_Position.xz + vec2(patchIndex.x, patchIndex.y)) / vec2(u_PatchCount.x, u_PatchCount.y);
	vec2 texUV2 = texUV * u_MapSize;

	vs_out.TexUV = texUV2 / u_HeightMapTextureSize;
	vs_out.MapIndex = layer;
	gl_Position = vec4(texUV2.x, 0, texUV2.y, 1);
}

