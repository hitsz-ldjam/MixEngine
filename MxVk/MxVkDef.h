#pragma once
#ifndef MX_VK_DEF_H
#define MX_VK_DEF_H_

#include "../Mx/MxDef.h"
#include "../Mx/MxObject.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <optional>


namespace Mix {
    namespace Graphics {

        typedef size_t ArrayIndex;
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

        struct SwapchainSupportDetails {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };

        struct Vertex {
            glm::vec3 pos;
            glm::vec3 normal;
            glm::vec2 uv;

            //get vertex input description
            static std::vector<vk::VertexInputBindingDescription>& GetBindingDescrip() {
                static std::vector<vk::VertexInputBindingDescription> bindingDescription = {
                vk::VertexInputBindingDescription(0,sizeof(Vertex),vk::VertexInputRate::eVertex) };

                return bindingDescription;
            }



            //get vertex input attributi description
            static std::vector<vk::VertexInputAttributeDescription>& GetAttributeDescrip() {
                static std::vector<vk::VertexInputAttributeDescription> attributeDescription = {
                    vk::VertexInputAttributeDescription(0,0,vk::Format::eR32G32B32Sfloat,offsetof(Vertex, pos)),
                    vk::VertexInputAttributeDescription(1,0,vk::Format::eR32G32B32Sfloat,offsetof(Vertex, normal)),
                    vk::VertexInputAttributeDescription(2,0,vk::Format::eR32G32Sfloat,offsetof(Vertex, uv))
                };

                return attributeDescription;
            }
        };

    }
}
#endif // !MX_VK_DEF_H
