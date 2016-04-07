uniform ivec2 u_PatchCount;
uniform ivec2 u_MapSize;
uniform vec3 u_Scale;
uniform int u_PatchPerTexture;
uniform sampler2DArray u_HeightMap;

out VS_OUT
{
	vec2 TexUV;
	int InstanceId;
	ivec2 PatchIndex;
} vs_out;

void main()
{  
	const vec4 vertices[] = vec4[]
	(
		vec4(0.0, 0.0, 0.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
		vec4(1.0, 0.0, 1.0, 1.0)
	);

	ivec2 patchIndex = ivec2(gl_InstanceID % u_PatchCount.x, (gl_InstanceID / u_PatchCount.x) % u_PatchCount.y);

	//vs_out.TexUV = (vertices[gl_VertexID].xz / u_PatchCount) + (patchIndex * 64 / u_MapSize);
	vs_out.TexUV = (vertices[gl_VertexID].xz / u_PatchCount);

	float y = texture(u_HeightMap, vec3(vs_out.TexUV, gl_InstanceID / u_PatchPerTexture)).r;
	vec3 pos = (vertices[gl_VertexID].xyz + vec3(patchIndex.x, y, patchIndex.y)) * u_Scale;

	vs_out.InstanceId = gl_InstanceID;
	gl_Position = vec4(pos, 1);
	vs_out.PatchIndex = patchIndex;
}

