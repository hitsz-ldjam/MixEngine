#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
	vec3 position;
	vec3 forward;
	mat4 viewMat;
    mat4 projMat;
}camera;

layout(set = 0, binding = 1) uniform MeshUniform {
	mat4 modelMat;
	mat4 normMat;
}mesh;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

#define EPSILON 0.01

void main() 
{
    gl_Position = camera.projMat * camera.viewMat * mesh.modelMat * vec4(inPosition, 1.0f);
	outNormal= mat3(mesh.normMat) * inNormal;
	outUV=vec2(inTexCoord);
}