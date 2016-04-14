layout(location = 0) out vec4 outPosition;
layout(location = 1) out uvec3 outData;

uniform vec3 u_Scale;
uniform float u_TexScale = 0.05;
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
	vec3 normal = normalize(fs_in.Normal);

	vec3 blendWeights = pow(abs(normal), vec3(u_TriplanarBlendSharpness));
	blendWeights = blendWeights / (blendWeights.x + blendWeights.y + blendWeights.z);

	float d = length(fs_in.WorldPosition);
	float k = floor(log(d / 7.5) / log(2));
	float texScale = pow(0.5, k - 2);
	vec3 uvs = u_TexScale * texScale * fs_in.WorldPosition;
	//vec3 uvs = u_TexScale * fs_in.WorldPosition;

	vec3 diffuseY = texture(u_DiffuseMap, vec3(uvs.xz, 2)).xyz;
	vec3 diffuseX = texture(u_DiffuseMap, vec3(uvs.zy, 2)).xyz;
	vec3 diffuseZ = texture(u_DiffuseMap, vec3(uvs.xy, 2)).xyz;

	vec3 diffuse = (blendWeights.y * diffuseY) + (blendWeights.x * diffuseX) + (blendWeights.z * diffuseZ);

	outData = uvec3(packUnorm4x8(vec4(diffuse, TERRAIN_RENDERER_ID / 255)), 0, 0);
	outPosition = vec4( fs_in.ViewPosition, uintBitsToFloat(packHalf2x16(fs_in.ViewNormal.xy)) );
}


