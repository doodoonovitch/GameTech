layout (location = POSITION_ATTRIBUTE) in vec3 in_Position;

out VS_OUT
{
	//vec3 ViewDir;
	vec2 TexUV;
} vs_out;

void main()
{  
	vs_out.TexUV = (in_Position.xy + vec2(1.0)) * 0.5;
	gl_Position = vec4(in_Position, 1);

	//float fovy = u_DepthRangeFovYAspect.z;
	//float aspect = u_DepthRangeFovYAspect.w;
	//float tanFov2 = tan(fovy/2.0);
	//vec2 halfSizeNearPlane = vec2(tanFov2 * aspect, tanFov2);// * u_DepthRangeFovYAspect.x;	
	//vs_out.ViewDir = vec3((2.0 * halfSizeNearPlane * vs_out.TexUV) - halfSizeNearPlane, -1.0);
}

