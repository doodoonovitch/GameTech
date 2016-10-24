
// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi)
{
	vec4 temp;

	temp = texture(u_gBufferNormal, fs_in.TexUV, 0);
	fi.Normal = normalize(temp.xyz);

	float depth = texture(u_gDepthMap, fs_in.TexUV).r;

	float n = u_NearFarFovYAspect.x;
	float f = u_NearFarFovYAspect.y;
	depth = depth * 2.0 - 1.0;
	float zView = 2 * f * n / (depth * (f - n) - (f + n));

	//float zView = u_ProjMatrix[3][2] / (2.0 * depth - 1.0 - u_ProjMatrix[2][2]);

	vec2 xyView = fs_in.ViewRay * -zView;
	fi.Position = vec3(xyView, zView);

	uvec4 data = texture(u_gBufferAlbedoAndStatus, fs_in.TexUV, 0);
	fi.RendererId = int(data.w & 15);

	uvec3 matD = uvec3(data.x & Mask_0x000000FF, (data.x >> 8) & Mask_0x000000FF, (data.x >> 16) & Mask_0x000000FF);
	fi.DiffuseMaterial = data.xyz / 255.f;

	
	temp = texture(u_gBufferSpecularRoughness, fs_in.TexUV, 0);
	fi.Roughness = temp.w;
	fi.SpecularMaterial = temp.xyz;

	fi.EmissiveMaterial = texture(u_gBufferEmissive, fs_in.TexUV, 0).xyz;
}
