layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

struct TerrainPerInstanceData
{
	ivec4 mMatIndex;
	vec3 mOrigin;
};

layout (std430, binding = 0) buffer PerInstanceData
{
	TerrainPerInstanceData u_PerInstanceData[];
};


uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform vec3 u_Scale;

out VS_OUT
{
	vec2 TexUV;
	flat int MapIndex;
} vs_out;

void main()
{  
	int patchPerTexture = u_PatchCount.x * u_PatchCount.y;
	int localIndex = gl_InstanceID % patchPerTexture;
	int layer = gl_InstanceID / patchPerTexture;

	ivec2 patchIndex = ivec2(localIndex % u_PatchCount.x, localIndex / u_PatchCount.x);

	vec2 texUV = (in_Position.xz + vec2(patchIndex.x, patchIndex.y)) / vec2(u_PatchCount.x, u_PatchCount.y);

	vec2 texUV2 = texUV * u_MapSize;
	vec3 pos = vec3(texUV2.x, 0, texUV2.y) * u_Scale + u_PerInstanceData[layer].mOrigin;

	vs_out.TexUV = texUV;
	vs_out.MapIndex = layer;
	gl_Position = vec4(pos, 1);
}

