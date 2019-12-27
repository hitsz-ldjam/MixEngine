#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraUniform {
	vec3 cameraPos;
	mat4 viewMat;
    mat4 projMat;
}camera;

// Input
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Instancing
layout(location = 3) in vec4 col0;
layout(location = 4) in vec4 col1;
layout(location = 5) in vec4 col2;
layout(location = 6) in vec4 col3;
								
// Output
layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outUV;

#define EPSILON 0.01

mat3 getNormalMatFromModelMat(mat4 modelMat){
	mat3 result = mat3(inverse(transpose(modelMat)));
	result[0] = normalize(result[0]);
	result[1] = normalize(result[1]);
	result[2] = normalize(result[2]);
	return result;
}

void main() 
{
	mat4 modelMat;
	modelMat[0] = col0;
	modelMat[1] = col1;
	modelMat[2] = col2;
	modelMat[3] = col3;

    gl_Position = camera.projMat * camera.viewMat * modelMat * vec4(inPosition, 1.0f);
	outNormal= getNormalMatFromModelMat(modelMat) * inNormal;
	outUV=vec2(inTexCoord);
}