layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform samplerBuffer perInstanceDataSampler;

in VS_OUT
{
	vec3 Normal;
	int InstanceId;
} gs_in[3];

uniform float u_Magnitude = 0.1f;

void main()
{  
	int index = gs_in[0].InstanceId * 4;
	vec4 col1 = texelFetch(perInstanceDataSampler, index);
	vec4 col2 = texelFetch(perInstanceDataSampler, index + 1);
	vec4 col3 = texelFetch(perInstanceDataSampler, index + 2);
	vec4 col4 = texelFetch(perInstanceDataSampler, index + 3);
	
	mat4 modelMatrix = mat4(col1, col2, col3, col4);

	mat4 modelViewMatrix = u_ViewMatrix * modelMatrix;

	for(int i = 0; i < gl_in.length(); ++i )
	{
		vec4 position = modelViewMatrix * gl_in[i].gl_Position;
		gl_Position = u_ProjMatrix * position;
		EmitVertex();

		vec4 normal = modelViewMatrix * vec4(gs_in[i].Normal.xyz, 0);
		gl_Position = u_ProjMatrix * (position + normal * u_Magnitude);
		EmitVertex();

		EndPrimitive();
	}
}
