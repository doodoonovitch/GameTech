layout (std430, binding = 0) /*coherent writeonly*/ buffer TargetBuffer
{
	vec4 u_Target[];
};

//layout (binding = 0, rgba32f) uniform image1D u_TargetBuffer;

layout (std430, binding = 1) /*coherent readonly*/ buffer SourceBuffer
{
	vec4 u_Source[];
};

uniform uint u_TargetStride;
uniform uint u_SourceStride;


layout (local_size_x = 1, local_size_y = 1) in;
//layout (local_size_x = 32, local_size_y = 32) in;
//layout (local_size_x = MAX_COMPUTE_WORKGROUP_SIZE_X, local_size_y = MAX_COMPUTE_WORKGROUP_SIZE_Y) in;

void main(void)
{
	uint index = gl_GlobalInvocationID.x * u_SourceStride;
	
	DualQuat modelDQ;
	modelDQ.Qr = u_Source[index];
	modelDQ.Qd = u_Source[index + 1];
	vec3 scale = u_Source[index + 2].xyz;
	DualQuat viewModelDQ = dqMul(u_ViewDQ, modelDQ);
	
	int outIndex = int(gl_GlobalInvocationID.x * u_TargetStride);

	u_Target[outIndex] = viewModelDQ.Qr;
	u_Target[outIndex + 1] = viewModelDQ.Qd;
	u_Target[outIndex + 2] = vec4(scale, 0.0f);

}