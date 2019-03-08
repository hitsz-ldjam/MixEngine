#pragma once
#ifndef _MX_VK_DEF_H_
#define _MX_VK_DEF_H_

#include"..\Mx\MxDef.h"
#include"..\Mx\MxObject.h"
#include"vulkan/vulkan.hpp"
#include <optional>


namespace Mix {
    namespace Graphics {

        typedef uint32_t QueueIndex;
        typedef uint32_t QueueFamilyIndex;


        struct QueueSet {
            std::optional<vk::Queue> present;
            std::optional<vk::Queue> graphics;
            std::optional<vk::Queue> compute;
            std::optional<vk::Queue> transfer;
        };


        struct QueueFamilyIndexSet {
            std::optional<QueueFamilyIndex> present;
            std::optional<QueueFamilyIndex> graphics;
            std::optional<QueueFamilyIndex> compute;
            std::optional<QueueFamilyIndex> transfer;
        };


        struct PhysicalDeviceInfo {
            vk::PhysicalDevice physicalDevice;
            vk::PhysicalDeviceProperties properties;
            vk::PhysicalDeviceFeatures features;
            vk::PhysicalDeviceMemoryProperties memoryProperties;
            std::vector<vk::ExtensionProperties> extensions;
            std::vector<vk::QueueFamilyProperties> queueFamilies;
            QueueFamilyIndexSet familyIndexSet;
        };

    }
}
#endif // !_MX_VK_DEF_H_
