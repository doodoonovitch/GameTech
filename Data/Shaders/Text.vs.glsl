layout (location = POSITION_ATTRIBUTE) in vec2 in_Position;
layout (location = UV_ATTRIBUTE) in ivec2 in_TexUV;

struct GlyphInfo
{
	uint mLayerIndexAndStatus;
	uint mXTexUV;
	uint mYTexUV;
	uint mCharacterSize;
};

struct CharacterInfo
{
	uint mGlyphInfoIndex;
	uint mLocation;
	uint mColor;
};


layout (std430, binding = 0) buffer GlyphInfoBuffer
{
	GlyphInfo data[];
} u_GlyphInfoBuffer;

layout (std430, binding = 1) buffer PerInstanceDataBuffer
{
	CharacterInfo data[];
} u_PerInstanceDataBuffer;


out VS_OUT
{
	vec4 Color;
	vec2 TexUV;
	//flat uint LayerIndexAndStatus;
	flat uint LayerIndex;
} vs_out;

//uvec2 unpackUInt2x16(uint packData)
//{
//	return uvec2(packData & Mask_0x0000FFFF, (packData >> 16) & Mask_0x0000FFFF);
//}

void main()
{  
	CharacterInfo ci = u_PerInstanceDataBuffer.data[gl_InstanceID];

	GlyphInfo gi = u_GlyphInfoBuffer.data[ci.mGlyphInfoIndex];

	vec2 size = vec2(gi.mCharacterSize & Mask_0x0000FFFF, (gi.mCharacterSize >> 16) & Mask_0x0000FFFF); 

	vec2 pos = unpackHalf2x16(ci.mLocation) + in_Position * size;

	//vs_out.LayerIndexAndStatus = gi.mLayerIndexAndStatus;
	vs_out.LayerIndex = (gi.mLayerIndexAndStatus & Mask_0x0000FFFF);
	uint status = (gi.mLayerIndexAndStatus >> 16) & Mask_0x0000FFFF;
	if (status == 0)
	{
		vs_out.Color = unpackUnorm4x8(ci.mColor);
		vec2 xTexUV = unpackHalf2x16(gi.mXTexUV);
		vec2 yTexUV = unpackHalf2x16(gi.mYTexUV);

		vs_out.TexUV = vec2(xTexUV[in_TexUV.x], yTexUV[in_TexUV.y]);
	}
	else
	{
		vs_out.Color = vec4(0);
		vs_out.TexUV = vec2(1);
		vs_out.LayerIndex = 0;
	}

	//gl_Position = vec4(2 * pos / u_BufferViewportSize.zw, 0, 1);
	gl_Position = u_OrthoProjMatrix * vec4(pos, 0, 1);
}

