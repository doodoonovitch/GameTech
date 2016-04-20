layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform vec3 u_Scale;
uniform sampler2DArray u_HeightMap;

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

	float height = texture(u_HeightMap, vec3(texUV, layer)).r;
	vec2 texUV2 = texUV * u_MapSize;
	vec3 pos = vec3(texUV2.x, 0, texUV2.y) * u_Scale;

	vs_out.TexUV = texUV;
	vs_out.MapIndex = layer;
	gl_Position = vec4(pos, 1);
}

