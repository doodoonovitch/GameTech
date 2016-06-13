layout (triangles, equal_spacing, ccw) in;



in TCS_OUT
{
	int InstanceId;
} tes_in[];

out TES_OUT
{
	//vec3 PatchDistance;
	flat int InstanceId;
} tes_out;

void main()
{
	vec4 p0 = gl_TessCoord.x * gl_in[0].gl_Position;
    vec4 p1 = gl_TessCoord.y * gl_in[1].gl_Position;
    vec4 p2 = gl_TessCoord.z * gl_in[2].gl_Position;

    gl_Position = normalize(p0 + p1 + p2);
	tes_out.InstanceId = tes_in[0].InstanceId;
    //tes_out.PatchDistance = gl_TessCoord;
}


