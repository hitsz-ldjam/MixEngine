#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform CameraUniform {
	vec3 cameraPos;
	mat4 viewMat;
    mat4 projMat;
}cameraUbo;

layout(set = 0, binding = 1) uniform ParamUniform {
	vec4 lightPos;
	vec4 lightColor;
	float exposure;
	float gamma;
	float prefilteredCubeMipLevels;
	float scaleIBLAmbient;
}paramUbo;

layout(set = 0, binding = 2) uniform samplerCube irradianceMap;
layout(set = 0, binding = 3) uniform samplerCube prefilteredMap;
layout(set = 0, binding = 4) uniform sampler2D brdfLutMap;

layout(set = 1, binding = 0) uniform sampler2D colorMap;
layout(set = 1, binding = 1) uniform sampler2D physicalDescriptorMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;
layout(set = 1, binding = 3) uniform sampler2D aoMap;
layout(set = 1, binding = 4) uniform sampler2D emissiveMap;

layout(push_constant) uniform MaterialUbo {
	layout(offset = 64) vec4 baseColorFactor;
	vec4 emissiveFactor;
	vec4 diffuseFactor;
	vec4 specularFactor;
	float workflow;
	float hasBaseColorTexture;
	float hasPhysicalDescriptorTexture;
	float hasNormalTexture;	
	float hasOcclusionTexture;	
	float hasEmissiveTexture;
	float metallicFactor;
	float roughnessFactor;
	float alphaMask;
	float alphaMaskCutoff;
} materialUbo;

layout(location = 0) out vec4 outColor;



/////////////////////////////////////////////////////////////
struct PbrInfo {
	float NdotL;
	float NdotV;
	float NdotH;
	float LdotH;
	float VdotH;
	float perceptualRoughness;
	float metalness;
	vec3 reflectance0;
	vec3 reflectance90;
	float alphaRoughness;
	vec3 diffuseColor;
	vec3 specularColor;
};

/////////////////////////////////////////////

const float PI = 3.141592653589793;
const float minRoughness = 0.04;

const int PBR_WORKFLOW_METALLIC_ROUGHNESS = 0;
const int PBR_WORKFLOW_SPECULAR_GLOSINESS = 1;

/////////////////////////////////////////////

#define MX_USE_SRGB 1
#define MX_USE_HDR 1

/////////////////////////////////////////////

vec3 uncharted2Tonemap(vec3 color) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec4 tonemap(vec4 color) {
	vec3 outcol = uncharted2Tonemap(color.rgb * paramUbo.exposure);
	outcol = outcol * (1.0f / uncharted2Tonemap(vec3(11.2f)));
	return vec4(pow(outcol, vec3(1.0f / paramUbo.gamma)), color.a);
}

vec4 srgbToLinear(vec4 srgb) {
#if MX_USE_SRGB
	return vec4(pow(srgb.xyz, vec3(2.2)), srgb.a);
#else
	return srgb;
#endif
}

vec4 linearToSrgb(vec4 linear){
#if MX_USE_SRGB
	return vec4(pow(linear.xyz, vec3(1.0f / paramUbo.gamma)), linear.a);
#else
	return linear;
#endif
}

// find the normal for this fragment, pull either from a predefined normal map or from the 
// interpolated mesh normal and tangent attributes
vec3 getNormal() {
	vec3 tangentNormal = texture(normalMap, inUV).xyz * 2.0f - 1.0f;

	vec3 q1 = dFdx(inWorldPos);
	vec3 q2 = dFdy(inWorldPos);
	vec2 st1 = dFdx(inUV);
	vec2 st2 = dFdy(inUV);

	vec3 N = normalize(inNormal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

// calculation of the lighting contribution from an optional image base light source
vec3 getIBLContrbution(PbrInfo pbrInfo, vec3 n, vec3 reflection) {
	float lod = (pbrInfo.perceptualRoughness * paramUbo.prefilteredCubeMipLevels);
	vec3 brdf = (texture(brdfLutMap, vec2(pbrInfo.NdotV, 1.0 - pbrInfo.perceptualRoughness))).rgb;

#if MX_USE_HDR
	vec3 diffuseLight = srgbToLinear(tonemap(texture(irradianceMap, n))).rgb;
	vec3 specularLight = srgbToLinear(tonemap(textureLod(prefilteredMap, reflection, lod))).rgb;
#else
	vec3 diffuseLight = texture(irradianceMap, n).rgb;
	vec3 specularLight = textureLod(prefilteredMap, reflection, lod).rgb;
#endif

	vec3 diffuse = diffuseLight * pbrInfo.diffuseColor;
	vec3 specular = specularLight * (pbrInfo.specularColor * brdf.x + brdf.y);

	diffuse *= paramUbo.scaleIBLAmbient;
	specular *= paramUbo.scaleIBLAmbient;

	return diffuse + specular;
}

// lambertian diffuse
vec3 diffuse(PbrInfo pbrInfo) {
	return pbrInfo.diffuseColor / PI;
}

// the following equation models the fresnel reflectance term of the spec quation F()
vec3 specularReflection(PbrInfo pbrInfo) {
		return pbrInfo.reflectance0 + (pbrInfo.reflectance90 - pbrInfo.reflectance0) * pow(clamp(1.0 - pbrInfo.VdotH, 0.0, 1.0), 5.0);
}

// this calculated the specular geometric attenuation G()
float geometrixOcclision(PbrInfo pbrInfo) {
	float NdotL = pbrInfo.NdotL;
	float NdotV = pbrInfo.NdotV;
	float r = pbrInfo.alphaRoughness;

	float attenuationL = 2.0f * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0f * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

// the following equation model the dirtribution of microfacet normals across the are being drawn D()
float microfacetDistribution(PbrInfo pbrInfo) {
	float roughnessSq = pbrInfo.alphaRoughness * pbrInfo.alphaRoughness;
	float f = (pbrInfo.NdotH * roughnessSq - pbrInfo.NdotH) * pbrInfo.NdotH + 1.0f;
	return roughnessSq / (PI * f * f);
}

// get metallic factor from specular glossiness workflow inputs
float convertMetallic(vec3 diffuse, vec3 specular, float maxSpecular) {
	float perceivedDiffuse = sqrt(0.299 * diffuse.r * diffuse.r + 0.587 * diffuse.g * diffuse.g + 0.114 * diffuse.b * diffuse.b);
	float perceivedSpecular = sqrt(0.299 * specular.r * specular.r + 0.587 * specular.g * specular.g + 0.114 * specular.b * specular.b);
	if(perceivedSpecular < minRoughness) {
		return 0.0;
	}
	float a = minRoughness;
	float b = perceivedDiffuse * (1.0 - maxSpecular) / (1.0 - minRoughness) + perceivedSpecular - 2.0 * minRoughness;
	float c = minRoughness - perceivedSpecular;
	float d = max(b * b - 4.0 * a * c, 0.0);
	return clamp((-b + sqrt(d)/(2.0 * a)), 0.0, 1.0);
}

void main() {
	float perceptualRoughness;
	float metallic;
	vec3 diffuseColor;
	vec4 baseColor;

	vec3 f0 = vec3(0.04);

	if(materialUbo.alphaMask == 1.0f) {
		if(materialUbo.hasBaseColorTexture == 1.0f) {
			baseColor = srgbToLinear(texture(colorMap, inUV)) * materialUbo.baseColorFactor;
		} else {
			baseColor = materialUbo.baseColorFactor;
		}
		if(baseColor.a < materialUbo.alphaMaskCutoff) {
			discard;
		}
	}

	if(materialUbo.workflow == PBR_WORKFLOW_METALLIC_ROUGHNESS) {
		perceptualRoughness = materialUbo.roughnessFactor;
		metallic = materialUbo.metallicFactor;
		if(materialUbo.hasPhysicalDescriptorTexture == 1.0f) {
			vec4 mrSample = texture(physicalDescriptorMap, inUV);
			
			perceptualRoughness = mrSample.g * perceptualRoughness;
			metallic = mrSample.b * metallic;
		} else {
			perceptualRoughness = clamp(perceptualRoughness, minRoughness, 1.0f);
			metallic = clamp(metallic, 0.0f, 1.0f);
		}

		if(materialUbo.hasBaseColorTexture == 1.0f) {
			baseColor = srgbToLinear(texture(colorMap, inUV)) * materialUbo.baseColorFactor;
		} else {
			baseColor = materialUbo.baseColorFactor;
		}
	}

	if(materialUbo.workflow == PBR_WORKFLOW_SPECULAR_GLOSINESS) {
		if(materialUbo.hasPhysicalDescriptorTexture == 1.0f) {
			perceptualRoughness = 1.0 - texture(physicalDescriptorMap, inUV).a;
		} else {
			perceptualRoughness = 0.0;
		}

		const float epsilon = 1e-6;

		vec4 diffuse = srgbToLinear(texture(colorMap, inUV));
		vec3 specular = srgbToLinear(texture(physicalDescriptorMap, inUV)).rgb;

		float maxSpecular = max(max(specular.r, specular.g), specular.b);

		metallic = convertMetallic(diffuse.rgb, specular, maxSpecular);

		vec3 baseColorDiffusePart = diffuse.rgb * ((1.0 - maxSpecular) / (1.0 - minRoughness) / max(1.0 - metallic, epsilon)) * materialUbo.diffuseFactor.rgb;
		vec3 baseColorSpecularPart = specular - (vec3(minRoughness) * (1.0 - metallic) * (1.0 / max(metallic, epsilon))) * materialUbo.specularFactor.rgb;
		baseColor = vec4(mix(baseColorDiffusePart, baseColorSpecularPart, metallic * metallic), diffuse.a) * materialUbo.baseColorFactor;
	}

	diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
	diffuseColor *= 1.0 - metallic;

	float alphaRoughness = perceptualRoughness * perceptualRoughness;
	vec3 specularColor = mix(f0, baseColor.rgb, metallic);

	// compute reflectance
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;

	vec3 cameraPos = vec3(cameraUbo.cameraPos);
	vec3 n = (materialUbo.hasNormalTexture == 1.0f) ? getNormal() : normalize(inNormal);
	vec3 v = normalize(cameraPos - inWorldPos);
	vec3 l = normalize(paramUbo.lightPos.xyz - inWorldPos);
	vec3 h = normalize(l + v);
	vec3 reflection = normalize(reflect(v, n));

	float NdotL = clamp(dot(n, l), 0.001, 1.0);
	float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);
	float NdotH = clamp(dot(n, h), 0.0, 1.0);
	float LdotH = clamp(dot(l, h), 0.0, 1.0);
	float VdotH = clamp(dot(v, h), 0.0, 1.0);

	PbrInfo pbrInfo = PbrInfo(
		NdotL,
		NdotV,
		NdotH,
		LdotH,
		VdotH,
		perceptualRoughness,
		metallic,
		specularEnvironmentR0,
		specularEnvironmentR90,
		alphaRoughness,
		diffuseColor,
		specularColor
	);

	// calculate the shading terms for the microfacet specular shading model
	vec3 F = specularReflection(pbrInfo);
	float G = geometrixOcclision(pbrInfo);
	float D = microfacetDistribution(pbrInfo);

	const vec3 lightColor = paramUbo.lightColor.rgb;

	// calculation of analytical lighting contribution
	vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInfo);
	vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);

	// calculate lighting contribution form image based lighting source
	vec3 color = NdotL * lightColor * (diffuseContrib + specContrib);

	vec3 ambient = getIBLContrbution(pbrInfo, n, reflection);
	color += ambient;

	const float occlusionStrength = 1.0f;

	if(materialUbo.hasOcclusionTexture == 1.0f) {
		float ao = texture(aoMap, inUV).r;
		color = mix(color, color * ao, occlusionStrength);
	}

	vec3 emissiveFactor = materialUbo.emissiveFactor.xyz;
	if(materialUbo.hasEmissiveTexture  == 1.0f) {
		vec3 emissive = srgbToLinear(texture(emissiveMap, inUV)).rgb * emissiveFactor;
		color += emissive;
	}
	
	color = vec3(1.0) - exp(-color * paramUbo.exposure);

	outColor = vec4(color, baseColor.a);
	outColor = linearToSrgb(outColor);
}