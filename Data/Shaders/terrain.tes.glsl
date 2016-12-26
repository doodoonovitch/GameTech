layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
//layout (quads, equal_spacing) in;

uniform vec3 u_Scale;

uniform sampler2DArray u_HeightMap;
uniform sampler2DArray u_BlendMap;

in TCS_OUT
{
	vec2 TexUV;
	//float MipmapLevel;
	flat int MapIndex;
} tes_in[];

out TES_OUT
{
	vec4 Blend;
	vec3 Position;
	vec2 TexUV;
	//float MipmapLevel;
	flat int MapIndex;
} tes_out;

void main()
{
	vec2 tc1 = mix(tes_in[0].TexUV, tes_in[1].TexUV, gl_TessCoord.x);
	vec2 tc2 = mix(tes_in[2].TexUV, tes_in[3].TexUV, gl_TessCoord.x);
	vec2 tc = mix(tc2, tc1, gl_TessCoord.y);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,	gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p = mix(p2, p1, gl_TessCoord.y);
	//float mip1 = mix(tes_in[0].MipmapLevel, tes_in[1].MipmapLevel, gl_TessCoord.x);
	//float mip2 = mix(tes_in[2].MipmapLevel, tes_in[3].MipmapLevel, gl_TessCoord.x);
	//float mip = mix(mip2, mip1, gl_TessCoord.y);

	vec3 texCoord = vec3(tc, tes_in[0].MapIndex);
	float height = texture(u_HeightMap, texCoord).x;
	p.y += u_Scale.y * height;

	vec4 blend = texture(u_BlendMap, texCoord);
	blend /= (blend.x + blend.y + blend.z + blend.w).xxxx;

	tes_out.TexUV = tc;
	tes_out.Blend  = blend;

	tes_out.Position = p.xyz;
	tes_out.MapIndex = tes_in[0].MapIndex;
	//tes_out.MipmapLevel = mip;

	gl_Position = u_ViewProjMatrix * vec4(p.xyz, 1);
}


