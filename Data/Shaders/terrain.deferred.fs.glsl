layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec2 outData;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.01;
uniform float u_TriplanarBlendSharpness = 1.0;

uniform sampler2DArray u_HeightMap;
uniform sampler2DArray u_DiffuseMap;


in GS_OUT
{
	vec3 WorldPosition;
	vec3 ViewPosition;
	vec3 Normal;
	vec3 ViewNormal;
	flat int Layer;
} fs_in;

void main()
{
	outPosition = vec4(fs_in.ViewPosition, uintBitsToFloat(CombineRenderIdAndMaterialIndex(TERRAIN_RENDERER_ID, 0)));

	vec3 normal = normalize(fs_in.Normal);

	vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	vec3 uvs = u_TexScale * fs_in.WorldPosition;

	vec4 diffuseY = texture(u_DiffuseMap, vec3(uvs.xz, 2));
	vec4 diffuseX = texture(u_DiffuseMap, vec3(uvs.zy, 2));
	vec4 diffuseZ = texture(u_DiffuseMap, vec3(uvs.xy, 2));

	vec4 diffuse = (blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ);

	outData = uvec2(packUnorm4x8(diffuse), packHalf2x16(normalize(fs_in.ViewNormal).xy));
}


