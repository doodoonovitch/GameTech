uniform vec2 u_PatchCount;
uniform vec2 u_MapSize;
uniform vec3 u_Scale;

out VS_OUT
{
	vec2 TexUV;
	int InstanceId;
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

	ivec2 patchIndex = vec2(gl_InstanceID % u_PatchCount.x, (gl_InstanceID / u_PatchCount.x) % u_PatchCount.y);
	vec3 pos = (vertices[gl_VertexID].xyz + vec3(patchIndex.x, 0.0, patchIndex.y)) * u_Scale;
	vs_out.TexUV = (vertices[gl_VertexID].xz / u_PatchCount) + (patchIndex * 64 / u_MapSize);
	vs_out.InstanceId = gl_InstanceID;
	gl_Position = vec4(pos, 1);
}

