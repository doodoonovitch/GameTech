//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;



in TCS_OUT
{
	vec3 Center;
	float Radius;
} tes_in[];

out TES_OUT
{
	vec3 Normal;
} tes_out;

void main()
{
	vec3 p = gl_in[0].gl_Position.xyz;

	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;

	float phi = PI * ( u - .5 );
	float theta = 2. * PI * ( v - .5 );

	float cosphi = cos(phi);
	vec3 xyz = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );
	tes_out.Normal = xyz;

	xyz *= tes_in[0].Radius;
	xyz += tes_in[0].Center;

	gl_Position = u_ProjMatrix * vec4(xyz, 1.);
}


