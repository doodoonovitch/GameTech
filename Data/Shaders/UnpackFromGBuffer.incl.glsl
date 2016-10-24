

//vec3 ReconstructPosition(float zPixel)
//{
//	vec3 xyzNDC = vec3(gl_FragCoord.xy / u_BufferViewportSize.zw, zPixel) * vec3(2.0) - vec3(1.0);
//	float zEye = u_ProjMatrix[3][2] / (xyzNDC.z - u_ProjMatrix[2][2]); 

//	float n = u_NearFarFovYAspect.x;
//	//float f = u_NearFarFovYAspect.y;
//	////float l = u_LeftRightTopBottom.x;
//	////float r = u_LeftRightTopBottom.y;
//	////float t = u_LeftRightTopBottom.z;
//	////float b = u_LeftRightTopBottom.w;

//	////vec2 rt = u_LeftRightTopBottom.yz;
//	////vec2 lb = u_LeftRightTopBottom.xw;
//	vec2 rt = vec2(u_BufferViewportSize.z - 1.0, 0.0);
//	vec2 lb = vec2(0.0, u_BufferViewportSize.w - 1.0);

//	//float zEye = 2 * f * n / (xyzNDC.z * (f - n) - (f + n));
//	vec2 xyEye = vec2(-zEye) * (xyzNDC.xy * (rt - lb) + (rt + lb)) / vec2(2.0 * n);

//	return vec3(xyEye.x, xyEye.y, zEye);	
//}



// ---------------------------------------------------------------------------
// GBuffer data extraction helpers
//
// ---------------------------------------------------------------------------
void UnpackFromGBuffer(out FragmentInfo fi)
{
	//fi.Position = texture(u_gBufferPosition, fs_in.TexUV, 0).xyz;

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
	//fi.Position.z = zView;
	fi.Position = vec3(xyView, zView);

	//vec3 p = ReconstructPosition(temp.w);
	//fi.Position.z = p.z;
	//fi.Position = p;

	uvec4 data = texture(u_gBufferAlbedoAndStatus, fs_in.TexUV, 0);
	fi.RendererId = int(data.w & 15);

	uvec3 matD = uvec3(data.x & Mask_0x000000FF, (data.x >> 8) & Mask_0x000000FF, (data.x >> 16) & Mask_0x000000FF);
	fi.DiffuseMaterial = data.xyz / 255.f;

	
	temp = texture(u_gBufferSpecularRoughness, fs_in.TexUV, 0);
	fi.Roughness = temp.w;
	fi.SpecularMaterial = temp.xyz;

	fi.EmissiveMaterial = texture(u_gBufferEmissive, fs_in.TexUV, 0).xyz;
}
