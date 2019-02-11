#include "MxVkImage.h"

namespace Mix
{
	VkImage MxVkImage::createImage2D(const MxVkManager * manager, const VkExtent2D extent, const VkFormat format, const VkImageUsageFlags usage, const VkSampleCountFlagBits sampleCount, const VkImageTiling tiling, const VkImageLayout initialLayout, const VkSharingMode sharingMode, const uint32_t mipLevels, const uint32_t arrayLayers)
	{
		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.extent.width = extent.width;
		createInfo.extent.height = extent.height;
		createInfo.extent.depth = 1;
		createInfo.mipLevels = mipLevels;
		createInfo.arrayLayers = arrayLayers;
		createInfo.format = format;
		createInfo.tiling = tiling;
		createInfo.initialLayout = initialLayout;
		createInfo.usage = usage;
		createInfo.sharingMode = sharingMode;
		createInfo.samples = sampleCount;

		VkImage tempImage;
		MX_VK_CHECK_RESULT(vkCreateImage(manager->getDevice(), &createInfo, nullptr, &tempImage));
		return tempImage;
	}

	VkImageView MxVkImage::createImageView2D(const MxVkManager* manager, const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags, const uint32_t mipLevel, const uint32_t levelCount, const uint32_t layer, const uint32_t layerCount)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = mipLevel; //the first mipmap level accessible to the view
		viewInfo.subresourceRange.levelCount = levelCount; //the number of mipmap levels (starting from baseMipLevel) accessible to the view
		viewInfo.subresourceRange.baseArrayLayer = layer;
		viewInfo.subresourceRange.layerCount = layerCount;

		VkImageView imageView;
		MX_VK_CHECK_RESULT(vkCreateImageView(manager->getDevice(), &viewInfo, nullptr, &imageView));
		return imageView;
	}

	VkDeviceMemory MxVkImage::allocateImageMemory(const MxVkManager* manager, const VkImage image, const VkMemoryPropertyFlags properties)
	{
		VkMemoryRequirements memRequirements = {};
		vkGetImageMemoryRequirements(manager->getDevice(), image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = manager->getMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

		VkDeviceMemory tempMemory;
		MX_VK_CHECK_RESULT(vkAllocateMemory(manager->getDevice(), &allocInfo, nullptr, &tempMemory));
		return tempMemory;
	}

	MxVkImage * MxVkImage::createDepthStencil(const MxVkManager* manager, const VkFormat format, const VkExtent2D& extent, const VkSampleCountFlagBits sampleCount)
	{
		MxVkImage* image = new MxVkImage;

		image->image = MxVkImage::createImage2D(manager, extent, format,
													VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, sampleCount);
		image->memory = MxVkImage::allocateImageMemory(manager, image->image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vkBindImageMemory(manager->getDevice(), image->image, image->memory, 0);
		image->view = MxVkImage::createImageView2D(manager, image->image, format, VK_IMAGE_ASPECT_DEPTH_BIT);

		image->extent = extent;
		image->format = format;

		return image;
	}
}
