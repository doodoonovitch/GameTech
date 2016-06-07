//layout (quads, fractional_odd_spacing) in;
//layout (quads, fractional_even_spacing) in;
layout (quads, equal_spacing) in;



//in TCS_OUT
//{
//	vec3 Position;
//} tes_in[];

//out TES_OUT
//{
//	vec3 PatchDistance;
//} tes_out;

void main()
{
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position;
    vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position;
    vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position;

    gl_Position = vec4(normalize(p0 + p1 + p2), 1);
    //tes_out.PatchDistance = gl_TessCoord;
}


