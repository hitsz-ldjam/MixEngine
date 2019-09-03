#include "MxTexture.h"
#include "../../Object/MxObject.h"
#include "../../Vulkan/Buffers/MxVkBufferTransfer.h"
#include "../../Vulkan/CommandBuffer/MxVkCommanddBufferHandle.h"
#include "../../Vulkan/Buffers/MxVkBuffer.h"
#include "../MxGraphics.h"
#include "../../Vulkan/MxVulkan.h"

namespace Mix {
	Vulkan::WriteDescriptorSet Texture::getWriteDescriptor(uint32_t _binding,
	                                                       vk::DescriptorType _descriptorType,
	                                                       const std::optional<OffsetSize64>& _offsetSize) const {
		vk::DescriptorImageInfo imageInfo{ mSampler, mImageView, vk::ImageLayout::eShaderReadOnlyOptimal };

		vk::WriteDescriptorSet write{ nullptr, _binding, 0, 1,_descriptorType };
		return Vulkan::WriteDescriptorSet(write, imageInfo);
	}

	void Texture::apply(bool _updateMipmaps) {
		if (mTransfers.empty())
			return;

		auto& vulkan = Graphics::Get()->getRenderApi();
		Vulkan::CommandBufferHandle handle(vulkan.getTransferCommandPool());

		handle.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		vk::ImageMemoryBarrier barrier;
		barrier.image = mImage->get();
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;

		vk::BufferImageCopy copy;
		copy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		copy.imageSubresource.layerCount = 1;

		for (auto& pair : mTransfers) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			barrier.subresourceRange.baseMipLevel = pair.first.mipLevel;
			barrier.subresourceRange.baseArrayLayer = pair.first.layer;

			handle.get().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										 vk::PipelineStageFlagBits::eTransfer,
										 vk::DependencyFlags(),
										 nullptr, nullptr, barrier);

			copy.imageSubresource.mipLevel = pair.first.mipLevel;
			copy.imageSubresource.baseArrayLayer = pair.first.layer;
			copy.imageOffset = vk::Offset3D(pair.first.offset.x, pair.first.offset.y, 0);
			copy.imageExtent = vk::Extent3D(pair.first.extent.x, pair.first.extent.y, 1);



			handle.get().copyBufferToImage(pair.second->get(),
										   mImage->get(), vk::ImageLayout::eTransferDstOptimal,
										   copy);

			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			handle.get().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
										 vk::PipelineStageFlagBits::eTransfer,
										 vk::DependencyFlags(),
										 nullptr, nullptr, barrier);
		}

		if (_updateMipmaps && mipLevels() != 1)
			GenMipMap(handle.get(), *mImage, 0);

		handle.end();
		handle.submit();
		handle.wait();

		mTransfers.clear();
	}

	uint32_t Texture::width(uint32_t _mipLevel) const {
		return extent(_mipLevel).x;
	}

	uint32_t Texture::height(uint32_t _mipLevel) const {
		return extent(_mipLevel).y;
	}

	Math::Vector2i Texture::extent(uint32_t _mipLevel) const {
		return GetMipmapExtent(mImage->extent().width, mImage->extent().height, _mipLevel);
	}

	TextureFormat Texture::format() const {
		return FromVkFormat(mImage->format());
	}

	uint32_t Texture::GetMipmapLevel(uint32_t _width, uint32_t _height) {
		return static_cast<uint32_t>(std::floor(std::log2(std::max(_width, _height)))) + 1;
	}

	Math::Vector2ui Texture::GetMipmapExtent(uint32_t _width, uint32_t _height, uint32_t _mipLevel) {
		for (uint32_t i = 0; i < _mipLevel; ++i) {
			_width > 1 ? _width >>= 1 : 1;
			_height > 1 ? _height >>= 1 : 1;
		}
		return { _width, _height };
	}

	Texture::Texture(TextureType _type,
					 uint32_t _width, uint32_t _height, uint32_t _depth,
					 TextureFormat _format,
					 uint32_t _mipLevel, uint32_t _layer,
					 SamplerInfo _samplerInfo) : mType(_type) {
		auto& vulkan = Graphics::Get()->getRenderApi();
		auto& device = *vulkan.getLogicalDevice();

		// Calculate the appropriate mipLevel value
		// when _mipLevel is zero,
		if (_mipLevel == 0)
			_mipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(_width, _height)))) + 1;

		// vk::Image
		vk::ImageCreateInfo imageInfo;
		imageInfo.format = ToVkFormat(_format);
		imageInfo.extent = vk::Extent3D(_width, _height, _depth);
		imageInfo.mipLevels = _mipLevel;
		imageInfo.arrayLayers = _layer;
		imageInfo.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;


		// vk::ImageView
		vk::ImageViewCreateInfo viewInfo;
		viewInfo.format = ToVkFormat(_format);
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = _mipLevel;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = _layer;

		switch (mType) {
		case TextureType::Tex_2D:
			imageInfo.imageType = vk::ImageType::e2D;
			viewInfo.viewType = vk::ImageViewType::e2D;
			break;

		case TextureType::Cube:
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
			viewInfo.viewType = vk::ImageViewType::eCube;
			break;
		}

		mImage = std::make_shared<Vulkan::Image>(vulkan.getAllocator(), imageInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

		viewInfo.image = mImage->get();
		mImageView = device.get().createImageView(viewInfo);

		// Create vk::Sampler
		vk::SamplerCreateInfo samplerInfo;

		samplerInfo.addressModeU = ToVkSamplerAddressMode(_samplerInfo.wrapModeU);
		samplerInfo.addressModeV = ToVkSamplerAddressMode(_samplerInfo.wrapModeV);
		samplerInfo.addressModeW = ToVkSamplerAddressMode(_samplerInfo.wrapModeW);
		samplerInfo.magFilter = ToVkFilter(_samplerInfo.magFilter);
		samplerInfo.minFilter = ToVkFilter(_samplerInfo.minFilter);
		samplerInfo.mipmapMode = ToVkSampleMipMode(_samplerInfo.mipFilter);

		samplerInfo.minLod = 0;
		samplerInfo.maxLod = static_cast<float>(_mipLevel);

		mSampler = vulkan.getLogicalDevice()->get().createSampler(samplerInfo);

		vk::ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = _mipLevel;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = _layer;

		Vulkan::CommandBufferHandle handle(vulkan.getTransferCommandPool());
		handle.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		Vulkan::Image::TransferVkImageLayout(handle.get(), mImage->get(),
											 vk::ImageLayout::eUndefined,
											 vk::ImageLayout::eShaderReadOnlyOptimal,
											 subresourceRange);
		handle.end();
		handle.submit();
		handle.wait();
	}

	vk::Format Texture::ToVkFormat(TextureFormat _format) {
		switch (_format) {
		case TextureFormat::R8G8B8A8_Unorm: return vk::Format::eR8G8B8A8Unorm;
		case TextureFormat::B8G8R8A8_Unorm: return vk::Format::eB8G8R8A8Unorm;
		default:return vk::Format::eUndefined;
		}
	}

	TextureFormat Texture::FromVkFormat(vk::Format _format) {
		switch (_format) {
		case vk::Format::eR8G8B8A8Unorm:return TextureFormat::R8G8B8A8_Unorm;
		case vk::Format::eB8G8R8A8Unorm:return TextureFormat::B8G8R8A8_Unorm;
		}
	}

	vk::Filter Texture::ToVkFilter(TextureFilterMode _filterMode) {
		switch (_filterMode) {
		case TextureFilterMode::Nearest: return vk::Filter::eNearest;
		case TextureFilterMode::Linear: return vk::Filter::eLinear;
		default: return vk::Filter::eNearest;
		}
	}

	vk::SamplerMipmapMode Texture::ToVkSampleMipMode(TextureMipSampleMode _mipSampleMode) {
		switch (_mipSampleMode) {
		case TextureMipSampleMode::Nearest: return vk::SamplerMipmapMode::eNearest;
		case TextureMipSampleMode::Linear: return vk::SamplerMipmapMode::eLinear;
		default:return vk::SamplerMipmapMode::eNearest;
		}
	}

	vk::SamplerAddressMode Texture::ToVkSamplerAddressMode(TextureWrapMode _wrapMode) {
		switch (_wrapMode) {
		case TextureWrapMode::Repeat: return vk::SamplerAddressMode::eRepeat;
		case TextureWrapMode::ClampToEdge: return vk::SamplerAddressMode::eClampToEdge;
		case TextureWrapMode::Mirror: return vk::SamplerAddressMode::eMirroredRepeat;
		default:return vk::SamplerAddressMode::eRepeat;
		}
	}

	std::vector<char> Texture::GetPixels(const Vulkan::Image& _image,
										 uint32_t _x, uint32_t _y,
										 uint32_t _width, uint32_t _height,
										 const vk::ImageSubresourceLayers& _subresource) {
		auto& vulkan = Graphics::Get()->getRenderApi();
		Vulkan::CommandBufferHandle handle(vulkan.getTransferCommandPool());

		vk::ImageCreateInfo stagingCreateInfo;
		stagingCreateInfo.imageType = vk::ImageType::e2D;
		stagingCreateInfo.format = _image.format();
		stagingCreateInfo.extent = vk::Extent3D(_width, _height, 1);
		stagingCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
		stagingCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
		stagingCreateInfo.tiling = vk::ImageTiling::eLinear;
		stagingCreateInfo.mipLevels = 1;
		stagingCreateInfo.arrayLayers = _subresource.layerCount;

		auto staging = Vulkan::Image(vulkan.getAllocator(),
									 stagingCreateInfo,
									 vk::MemoryPropertyFlagBits::eHostVisible |
									 vk::MemoryPropertyFlagBits::eHostCoherent);

		vk::ImageMemoryBarrier dstBarrier;
		dstBarrier.image = staging.get();
		dstBarrier.subresourceRange.baseMipLevel = 0;
		dstBarrier.subresourceRange.levelCount = 1;
		dstBarrier.subresourceRange.baseArrayLayer = 0;
		dstBarrier.subresourceRange.layerCount = _subresource.layerCount;
		dstBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		dstBarrier.srcAccessMask = vk::AccessFlagBits();
		dstBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		dstBarrier.oldLayout = vk::ImageLayout::eUndefined;
		dstBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
		dstBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		dstBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vk::ImageMemoryBarrier srcBarrier;
		srcBarrier.image = _image.get();
		srcBarrier.subresourceRange.baseMipLevel = _subresource.mipLevel;
		srcBarrier.subresourceRange.levelCount = 1;
		srcBarrier.subresourceRange.baseArrayLayer = _subresource.baseArrayLayer;
		srcBarrier.subresourceRange.layerCount = _subresource.layerCount;
		srcBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		srcBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		srcBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		srcBarrier.srcAccessMask = vk::AccessFlagBits();
		srcBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
		srcBarrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		srcBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;

		handle.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		handle.get().pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
									 vk::PipelineStageFlagBits::eTransfer,
									 vk::DependencyFlags(),
									 nullptr, nullptr, { srcBarrier,dstBarrier });

		vk::ImageCopy copy;
		copy.extent = vk::Extent3D(_width, _height, 1);
		copy.srcSubresource = _subresource;
		copy.srcOffset = vk::Offset3D(_x, _y, 0);
		copy.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		copy.dstSubresource.mipLevel = 0;
		copy.dstSubresource.baseArrayLayer = 0;
		copy.dstSubresource.layerCount = _subresource.layerCount;

		handle.get().copyImage(_image.get(), vk::ImageLayout::eTransferSrcOptimal,
							   staging.get(), vk::ImageLayout::eTransferDstOptimal,
							   copy);

		srcBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		srcBarrier.dstAccessMask = vk::AccessFlagBits();
		srcBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		srcBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

		handle.get().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
									 vk::PipelineStageFlagBits::eBottomOfPipe,
									 vk::DependencyFlags(),
									 nullptr, nullptr, srcBarrier);

		handle.end();
		handle.submit();
		handle.wait();

		std::vector<char> result(staging.size());
		memcpy(result.data(), staging.rawPtr(), staging.size());
		return result;
	}

	void Texture::GenMipMap(const vk::CommandBuffer& _cmd, const Vulkan::Image& _image, uint32_t _layer) {
		auto& vulkan = Graphics::Get()->getRenderApi();
		auto support = vulkan.getPhysicalDevice()->checkFormatFeatureSupport(_image.format(), vk::ImageTiling::eOptimal,
																			 vk::FormatFeatureFlagBits::eBlitSrc | vk::FormatFeatureFlagBits::eBlitDst | vk::FormatFeatureFlagBits::eSampledImageFilterLinear);

		if (!support)
			return;

		vk::ImageMemoryBarrier barrier;
		barrier.image = _image.get();
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = _layer;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		auto mipWidth = _image.extent().width;
		auto mipHeight = _image.extent().height;

		// ImageLayout of each mipLevel is ShaderReadOnlyOptimal

		// change ImageLayout of mipLevel 0 to TransferSrcOptimal
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

		_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader,
							 vk::PipelineStageFlagBits::eTransfer,
							 vk::DependencyFlags(),
							 nullptr, nullptr, barrier);

		uint32_t i = 1;
		for (; i < _image.mipLevels() - 1; ++i) {
			// MipLevel		ImageLayout
			//		i-1		TransferSrc
			//	>	i		 ShaderRead  ->TransferDst
			//		i+1		 ShaderRead

			barrier.subresourceRange.baseMipLevel = i;
			barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader,
								 vk::PipelineStageFlagBits::eTransfer,
								 vk::DependencyFlags(),
								 nullptr, nullptr, barrier);

			vk::ImageBlit blit;
			blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
			blit.srcOffsets[1] = vk::Offset3D{ static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1 };
			blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = _layer;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
			blit.dstOffsets[1] = vk::Offset3D{ static_cast<int32_t>(mipWidth > 1 ? mipWidth >>= 1 : 1),
											   static_cast<int32_t>(mipHeight > 1 ? mipHeight >>= 1 : 1),
											   1 };
			blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = _layer;
			blit.dstSubresource.layerCount = 1;

			_cmd.blitImage(_image.get(), vk::ImageLayout::eTransferSrcOptimal,
						   _image.get(), vk::ImageLayout::eTransferDstOptimal,
						   blit, vk::Filter::eLinear);

			// MipLevel		ImageLayout
			//		i-1		TransferSrc  ->ShaderRead
			//	>	i		TransferDst
			//		i+1		 ShaderRead

			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
								 vk::PipelineStageFlagBits::eFragmentShader,
								 vk::DependencyFlags(),
								 nullptr, nullptr, barrier);

			// MipLevel		ImageLayout
			//		i-1		 ShaderRead
			//	>	i		TransferDst  ->TransferSrc
			//		i+1		 ShaderRead

			barrier.subresourceRange.baseMipLevel = i;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
								 vk::PipelineStageFlagBits::eTransfer,
								 vk::DependencyFlags(),
								 nullptr, nullptr, barrier);

			// MipLevel		ImageLayout
			//		i-1		 ShaderRead
			//	>	i		TransferSrc
			//		i+1		 ShaderRead
		}

		// MipLevel		ImageLayout
		//      ...		 ShaderRead
		//		n-2		TransferSrc
		//	>	n-1		 ShaderRead  ->TransferDst

		barrier.subresourceRange.baseMipLevel = i;
		barrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader,
							 vk::PipelineStageFlagBits::eTransfer,
							 vk::DependencyFlags(),
							 nullptr, nullptr, barrier);

		vk::ImageBlit blit;
		blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
		blit.srcOffsets[1] = vk::Offset3D{ static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1 };
		blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = _layer;
		blit.srcSubresource.layerCount = 1;

		blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
		blit.dstOffsets[1] = vk::Offset3D{ static_cast<int32_t>(mipWidth > 1 ? mipWidth >>= 1 : 1),
										   static_cast<int32_t>(mipHeight > 1 ? mipHeight >>= 1 : 1),
										   1 };
		blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = _layer;
		blit.dstSubresource.layerCount = 1;

		_cmd.blitImage(_image.get(), vk::ImageLayout::eTransferSrcOptimal,
					   _image.get(), vk::ImageLayout::eTransferDstOptimal,
					   blit, vk::Filter::eLinear);

		// MipLevel		ImageLayout
		//      ...		 ShaderRead
		//		n-2		TransferSrc  ->ShaderRead
		//	>	n-1		TransferDst

		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
							 vk::PipelineStageFlagBits::eFragmentShader,
							 vk::DependencyFlags(),
							 nullptr, nullptr, barrier);

		// MipLevel		ImageLayout
		//      ...		 ShaderRead
		//		n-2		 ShaderRead
		//	>	n-1		TransferDst  ->ShaderRead
		barrier.subresourceRange.baseMipLevel = i;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		_cmd.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
							 vk::PipelineStageFlagBits::eFragmentShader,
							 vk::DependencyFlags(),
							 nullptr, nullptr, barrier);

		// MipLevel		ImageLayout
		//		  0		 ShaderRead
		//		...		 ShaderRead
		//		n-1		 ShaderRead
	}

	Texture2D::Texture2D(uint32_t _width,
						 uint32_t _height,
						 TextureFormat _format,
						 uint32_t _mipLevel,
						 SamplerInfo _samplerInfo) :
		Texture(TextureType::Tex_2D, _width, _height, 1, _format, _mipLevel, 1, _samplerInfo) {
	}

	void Texture2D::setPixels(const void* _pixels, uint64_t _size, uint32_t _mipLevel) {
		if (_mipLevel < mipLevels()) {
			auto extent = GetMipmapExtent(width(), height(), _mipLevel);
			setPixels(_pixels, _size, 0, 0, extent.x, extent.y, _mipLevel);
		}
	}

	void Texture2D::setPixels(const void* _pixels,
							  uint64_t _size,
							  uint32_t _x,
							  uint32_t _y,
							  uint32_t _width,
							  uint32_t _height,
							  uint32_t _mipLevel) {
		if (_mipLevel < mipLevels()) {
			auto src = std::make_shared<Vulkan::Buffer>(mImage->getAllocator(), vk::BufferUsageFlagBits::eTransferSrc,
														vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
														_size, _pixels);

			mTransfers.emplace_back(std::piecewise_construct,
									std::forward_as_tuple(CopyToDstImageInfo{ _mipLevel, 0,Math::Vector2ui(_x,_y),Math::Vector2ui(_width,_height) }),
									std::forward_as_tuple(src));
		}
	}

	std::vector<char> Texture2D::getPixels(uint32_t _mipLevel) {
		if (_mipLevel >= mipLevels())
			return {};

		auto extent = GetMipmapExtent(width(), height(), _mipLevel);
		return GetPixels(*mImage, 0, 0, extent.x, extent.y, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, _mipLevel, 0, 1));
	}

	std::vector<char> Texture2D::getPixels(uint32_t _x,
										   uint32_t _y,
										   uint32_t _width,
										   uint32_t _height,
										   uint32_t _mipLevel) const {
		if (_mipLevel >= mipLevels())
			return {};

		return GetPixels(*mImage, _x, _y, _width, _height, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, _mipLevel, 0, 1));
	}

	CubeMap::CubeMap(uint32_t _width,
					 TextureFormat _format,
					 uint32_t _mipLevel,
					 SamplerInfo _samplerInfo) :
		Texture(TextureType::Cube, _width, _width, 1, _format, _mipLevel, 1, _samplerInfo) {
	}

	void CubeMap::apply(bool _updateMipmaps) {
		Texture::apply(_updateMipmaps);
	}

	void CubeMap::setPixels(const char* _pixels, uint64_t _size, CubeMapFace _face, uint32_t _mipLevel) {
		if (_mipLevel < mipLevels()) {
			auto extent = GetMipmapExtent(width(), height(), _mipLevel);
			setPixels(_pixels, _size, 0, 0, extent.x, extent.y, _face, _mipLevel);
		}
	}

	void CubeMap::setPixels(const char* _pixels, uint64_t _size,
							uint32_t _x, uint32_t _y,
							uint32_t _width, uint32_t _height,
							CubeMapFace _face, uint32_t _mipLevel) {
		if (_mipLevel >= mipLevels()) {
			auto src = std::make_shared<Vulkan::Buffer>(mImage->getAllocator(), vk::BufferUsageFlagBits::eTransferSrc,
														vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
														_size, _pixels);

			mTransfers.emplace_back(std::piecewise_construct,
									std::forward_as_tuple(CopyToDstImageInfo{ _mipLevel,GetFaceLayerIndex(_face),Math::Vector2ui(_x,_y),Math::Vector2ui(_width,_height) }),
									std::forward_as_tuple(src));
		}
	}

	std::vector<char> CubeMap::getPixels(CubeMapFace _face, uint32_t _mipLevel) {
		if (_mipLevel < mipLevels())
			return {};

		auto extent = GetMipmapExtent(width(), height(), _mipLevel);
		return GetPixels(*mImage, 0, 0, extent.x, extent.y, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, _mipLevel, GetFaceLayerIndex(_face), 1));
	}

	std::vector<char> CubeMap::getPixels(uint32_t _x, uint32_t _y,
										 uint32_t _width, uint32_t _height,
										 CubeMapFace _face, uint32_t _mipLevel) const {
		if (_mipLevel >= mipLevels())
			return {};

		return GetPixels(*mImage, _x, _y, _width, _height, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, _mipLevel, GetFaceLayerIndex(_face), 1));
	}

	uint32_t CubeMap::GetFaceLayerIndex(CubeMapFace _face) {
		switch (_face) {
		case CubeMapFace::PositiveX: return 1;
		case CubeMapFace::NegativeX: return 0;
		case CubeMapFace::PositiveY: return 3;
		case CubeMapFace::NegativeY: return 2;
		case CubeMapFace::PositiveZ: return 5;
		case CubeMapFace::NegativeZ: return 4;
		default:return 0;
		}
	}

	void CubeMap::genMipmaps(const vk::CommandBuffer& _cmd) const {
		auto& vulkan = Graphics::Get()->getRenderApi();
		auto support = vulkan.getPhysicalDevice()->checkFormatFeatureSupport(mImage->format(), vk::ImageTiling::eOptimal,
																			 vk::FormatFeatureFlagBits::eBlitSrc | vk::FormatFeatureFlagBits::eBlitDst | vk::FormatFeatureFlagBits::eSampledImageFilterLinear);
		if (support) {
			for (uint32_t face = 0; face < 6; ++face) {
				GenMipMap(_cmd, *mImage, face);
			}
		}
	}
}
