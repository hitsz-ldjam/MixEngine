#pragma once
#ifndef _MX_VULKAN_H_
#define _MX_VULKAN_H_

#ifndef VULKAN_H_ //if vulkan.h not included
#include<vulkan/vulkan.h>
#endif // !

#include"MxVkManager.h"
#include"MxVkCommandPool.h"
#include"MxVkDebug.h"
#include"MxVkDescriptor.h"
#include"MxVkFramebuffer.h"
#include"MxVkImage.h"
#include"MxVkPipeline.h"
#include"MxVkRenderPass.h"
#include"MxVkSwapchain.h"
#include"MxVkShader.hpp"
#include"MxVkBuffer.h"
#include"MxVkSyncObjectPool.h"
#include"MxVkUtils.h"

#include"MxVkModel.h"

#endif // !_MX_VULKAN_H_
