#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform CameraUniform {
	vec3 cameraPos;
	mat4 viewMat;
    mat4 projMat;
}camera;

layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform MeshConstant {
	vec4 mainColor;
	vec4 rimColor;
	float rimPower;
	float rimIntensity;
}RimParameter;

void main() 
{
	vec3 normal = normalize(inNormal);
	vec3 viewDir = normalize(camera.cameraPos - inPosition);

	vec4 color = RimParameter.mainColor;

	float rim = 1.0f - max(0.0f, dot(normal, viewDir));
	
	vec3 emmisive = RimParameter.rimColor.rgb * pow(rim, RimParameter.rimPower) * RimParameter.rimIntensity;

	outColor= vec4(color.rgb * (1 - rim) + emmisive, 1.0f);
}
