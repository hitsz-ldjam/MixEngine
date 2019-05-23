#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(set=1, binding = 1) uniform sampler2D mySampler;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() 
{
	//outColor=texture(mySampler,vec2(inUV));
	vec3 normal=normalize(inNormal);
	outColor=vec4(normal,1.0f);
}