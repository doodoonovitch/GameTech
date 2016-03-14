layout(location = 0) out uvec3 outPosition;
layout(location = 1) out vec4 outRGBA32UI;
layout(location = 2) out vec4 outAlbedo;

in GS_OUT
{
	vec4 Position;
	vec3 Normal;
	vec2 TexUV;
	flat int MaterialIndex;
} fs_in;

uniform sampler2D u_textureSampler;
uniform samplerBuffer u_materialDataSampler;


void main(void)
{
	outPosition = Position.xyz;

	int matIndex = fs_in.MaterialIndex;
	vec3 matData = texelFetch(u_materialDataSampler, matIndex);
	uint bitfieldValue = floatBitsToUint(v.w);
	int ambientTextureIndex = int(GetAmbientTextureIndex(bitfieldValue));

	vec4 materialAmbient = vec4(0);
	if (ambientTextureIndex != 0x000000FF)
	{
		materialAmbient = texture(u_textureSampler, fs_in.TexUV);
	}

	uvec4 data = uvec4(0);

	data.x = packHalf2x16(materialAmbient.xy);
	data.y = packHalf2x16(materialAmbient.z, fs_in.Normal.z);
	data.z = packHalf2x16(fs_in.Normal.xy);
	data.w = fs_in.MaterialIndex;

	outColor0 = data0;
	outColor1 = data1;
}
