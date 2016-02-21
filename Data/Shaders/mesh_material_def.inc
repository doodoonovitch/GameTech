layout (std140) uniform MESH_MATERIAL
{
	vec4 mAmbient;
	vec4 mDiffuse;
	vec4 mEmissive;
	vec4 mReflection;
	vec4 mSpecular;
	float mTransparency;
	float mShininess;
} MeshMaterial;