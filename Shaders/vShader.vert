#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObj
{
	mat4 model;
	mat4 view;
	mat4 proj;
}MVP;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outFragColor;

void main() 
{
    gl_Position = MVP.proj * MVP.view * MVP.model * vec4(inPosition, 1.0);
	outFragColor=vec3(abs(inNormal.x),abs(inNormal.y),abs(inNormal.z));
}