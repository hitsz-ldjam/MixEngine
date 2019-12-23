#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform CameraUniform {
	vec3 cameraPos;
	mat4 viewMat;
    mat4 projMat;
}cameraUbo;

#define MAX_NUM_JOINTS 128


layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

layout(push_constant) uniform MeshConstant {
	mat4 modelMat;
}mesh;

void main()
{
	vec4 locPos;
	
	locPos = mesh.modelMat * vec4(inPos, 1.0);
	outNormal = normalize(transpose(inverse(mat3(mesh.modelMat))) * inNormal);
	outWorldPos = locPos.xyz / locPos.w;
	outUV = inUV;
	gl_Position = cameraUbo.projMat * cameraUbo.viewMat * vec4(outWorldPos, 1.0f);
}


