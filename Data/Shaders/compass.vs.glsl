layout(location = POSITION_ATTRIBUTE) in vec4 in_Position;	//vertex position

uniform vec3 u_Translation;
uniform vec3 u_ViewDir;
//uniform mat4 u_Model;


out VS_OUT
{
	vec2 TexUV;
} vs_out;

void main()
{ 
	vec3 north = vec3(0, 0, 1);
	float angle = acos(dot(normalize(vec3(u_ViewDir.x, 0, u_ViewDir.z)), north));
	vec3 axis;
	if (u_ViewDir.x < north.x)
		axis = vec3(0, 0, 1);
	else
		axis = vec3(0, 0, -1);

	DualQuat dqT1 = dqFromTranslation(u_Translation);
	vec4 qRot = qAngleAxis(angle, axis);
	DualQuat dqRot = dqFromRotation(qRot);
	DualQuat dqT2 = dqFromTranslation(-u_Translation);
	DualQuat modelDQ = dqMul(dqMul(dqT1, dqRot), dqT2);

	vec4 p = vec4(dqTransformPoint(modelDQ, vec3(in_Position.xy, 0)), 1); 
	//vec4 p = vec4(in_Position.xy, 0, 1);
	gl_Position = u_OrthoProjMatrix * p;
	vs_out.TexUV = in_Position.zw;
}
