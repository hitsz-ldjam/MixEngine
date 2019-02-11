#pragma once
#ifndef _MX_VK_UTILS_H_
#define _MX_VK_UTILS_H_

#include<vulkan/vulkan.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include<stdexcept>
#include<string>



namespace Mix
{

	struct MxVkQueueFamilyIndices
	{
		uint32_t graphics;
		uint32_t present;
		uint32_t compute;
	};

	struct MxVkQueue
	{
		VkQueue graphics;
		VkQueue present;
		VkQueue compute;
	};


	std::string VkResultToString(const VkResult res);


}
#endif // !_MX_VK_UTILS_H_

//check the result returned by vkxxx
#define MX_VK_CHECK_RESULT(r)										\
{																	\
	VkResult __result=(r);											\
	if(__result != VK_SUCCESS)										\
	{																\
		throw std::runtime_error("Error : "+Mix::VkResultToString(__result)+" at "+__FILE__+" : "+std::to_string(__LINE__));  				\
	}																\
}

#define MX_FREE_OBJECT(p)											\
	if((p)!=nullptr)												\
		delete (p);