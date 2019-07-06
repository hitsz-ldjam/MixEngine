#pragma once
#ifndef MX_VK_IMAGE_H_
#define MX_VK_IMAGE_H_

#include <gli/gli.hpp>
#include "../Memory/MxVkAllocator.h"

namespace Mix {
	namespace Graphics {
		class CommandBufferHandle;
		class Image : GeneralBase::NoCopyBase {
		public:
			Image(const std::shared_ptr<DeviceAllocator>& _allocator,
				  const vk::ImageType _type,
				  const vk::MemoryPropertyFlags& _memProperty,
				  const vk::Extent3D& _extent,
				  const vk::Format _format,
				  const vk::ImageUsageFlags& _usage,
				  const uint32_t _mipLevels = 1,
				  const uint32_t _arrayLayers = 1,
				  const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
				  const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
				  const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
				  const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive,
				  const vk::ImageCreateFlagBits& _flag = {});

			Image(Image&& _other) noexcept { swap(_other); }

			Image& operator=(Image&& _other) noexcept { swap(_other); return *this; }

			void swap(Image& _other) noexcept;

			void uploadData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const;

			~Image();


			static std::shared_ptr<Image> CreateDepthStencil(const std::shared_ptr<DeviceAllocator>& _allocator,
															 const vk::Extent2D& _extent,
															 const vk::SampleCountFlagBits _sampleCount);

			static vk::Image CreateVkImage(const vk::Device& _device,
										   const vk::ImageType _type,
										   const vk::Extent3D& _extent,
										   const vk::Format _format,
										   const vk::ImageUsageFlags& _usage,
										   const uint32_t _mipLevels = 1,
										   const uint32_t _arrayLayers = 1,
										   const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
										   const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
										   const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal, 
										   const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive, 
										   const vk::ImageCreateFlagBits& _flag = {});

			static vk::UniqueImage CreateVkImageUnique(const vk::Device& _device,
												 const vk::ImageType _type,
												 const vk::Extent3D& _extent,
												 const vk::Format _format,
												 const vk::ImageUsageFlags& _usage,
												 const uint32_t _mipLevels = 1,
												 const uint32_t _arrayLayers = 1,
												 const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
												 const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
												 const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
												 const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive,
												 const vk::ImageCreateFlagBits& _flag = {});

			static vk::ImageView CreateVkImageView2D(const vk::Device& _device,
													 const vk::Image& _image,
													 const vk::Format _format,
													 const vk::ImageAspectFlags& _aspectFlags,
													 const uint32_t _baseMipLevel = 0,
													 const uint32_t _levelCount = 1,
													 const uint32_t _baseLayer = 0,
													 const uint32_t _layerCount = 1);

			static vk::UniqueImageView CreateVkImageView2DUnique(const vk::Device& _device,
													 const vk::Image& _image,
													 const vk::Format _format,
													 const vk::ImageAspectFlags& _aspectFlags,
													 const uint32_t _baseMipLevel = 0,
													 const uint32_t _levelCount = 1,
													 const uint32_t _baseLayer = 0,
													 const uint32_t _layerCount = 1);

			static void TransferVkImageLayout(const vk::CommandBuffer& _cmdbuffer,
											  vk::Image _image,
											  vk::ImageLayout _oldImageLayout,
											  vk::ImageLayout _newImageLayout,
											  const vk::ImageSubresourceRange& _subresourceRange,
											  vk::PipelineStageFlags _srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
											  vk::PipelineStageFlags _dstStageMask = vk::PipelineStageFlagBits::eAllCommands);

			const vk::ImageType& type() const { return mType; }

			const vk::Image& get() const { return mImage; }

			const vk::Format& format() const { return mFormat; }

			const vk::Extent3D& extent() const { return mExtent; }

			const uint32_t& mipLevels() const { return mMipLevels; }

			const uint32_t& arrayLevels() const { return mArrayLevels; }

			const vk::ImageLayout& layout() const { return mLayout; }

			const vk::ImageTiling& tiling() const { return mTiling; }

		private:
			std::shared_ptr<DeviceAllocator> mAllocator;

			vk::ImageType mType;
			vk::Image mImage;

			vk::Format mFormat;
			vk::Extent3D mExtent;

			MemoryBlock mMemory;

			uint32_t mMipLevels;
			uint32_t mArrayLevels;
			vk::ImageLayout mLayout;
			vk::ImageTiling mTiling;
		};

		//class ImageMgr :public GraphicsComponent {
		//    struct TempData {
		//        std::string name;
		//        ImageInfo imageInfo;
		//        const char* ptr;

		//        TempData() : ptr(nullptr) {
		//        }

		//        TempData(std::string _name, const ImageInfo& _imageInfo, const char* _p) :
		//            name(std::move(_name)), imageInfo(_imageInfo), ptr(_p) {
		//        }
		//    };
		//public:
		//    ~ImageMgr() {
		//        destroy();
		//    }
		//    void init(std::shared_ptr<Core>& _core, std::shared_ptr<DeviceAllocator>& _allocator);

		//    void beginLoad(const vk::CommandBuffer& _cmd);

		//    void loadTexture(const std::string& _name, const gli::texture& _texture);
		//    // void loadTexture(const Utils::ImageData& _imageData);
		//    void endLoad();

		//    const ImageInfo& getImage(const std::string& name) {
		//        if (mImageInfos.count(name) == 0)
		//            throw std::runtime_error("Error : No image named [ " + name + " ]");

		//        return mImageInfos[name].first;
		//    }

		//    void destroy();
		//private:
		//    std::vector<TempData> mDatas;
		//    bool mBegin = false;
		//    vk::DeviceSize mCurrSize = 0;
		//    vk::CommandBuffer mCmd;
		//    bool mCmdBegin = false;

		//    vk::DeviceSize mBufferSize = 16 * 1024 * 1024;

		//    vk::Buffer mStagingBuffer;
		//    MemoryBlock mMemBlock;
		//    vk::MemoryRequirements mMemReq;
		//    Fence mFence;

		//    std::unordered_map<std::string, std::pair<ImageInfo, MemoryBlock>> mImageInfos;

		//    std::shared_ptr<DeviceAllocator> mpAllocator;

		//    void flush();

		//    static vk::Format GliToVulkanFormat(gli::format _format);

		//    void loadImage2D(const std::string& _name, const gli::texture& _texture);
		//    /*
		//    void loadImage2DArray(const std::string& name, const gli::texture& texture);

		//    void loadImage1D(const std::string& name, const gli::texture& texture);
		//    void loadImage1DArray(const std::string& name, const gli::texture& texture);

		//    void loadImage3D(const std::string& name, const gli::texture& texture);

		//    void loadImageCube(const std::string& name, const gli::texture& texture);
		//    */
		//};
	}
}

#endif // !MX_VK_IMAGE_H_
