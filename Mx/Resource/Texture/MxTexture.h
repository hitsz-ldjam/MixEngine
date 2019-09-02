#pragma once
#pragma once
#ifndef MX_TEXTURE_H_
#define MX_TEXTURE_H_

#include "../../Vulkan/Image/MxVkImage.h"
#include "../MxResourceBase.h"
#include "../../Vulkan/Descriptor/MxVkDescriptor.h"
#include "../../Math/MxVector2.h"
#include "../../Definitions/MxCommonType.h"
#include <memory>

namespace Mix {
	namespace Vulkan {
		class Buffer;
	}

	struct SamplerInfo {
		TextureFilterMode minFilter = TextureFilterMode::Nearest;
		TextureFilterMode magFilter = TextureFilterMode::Nearest;
		TextureMipSampleMode mipFilter = TextureMipSampleMode::Nearest;

		TextureWrapMode wrapModeU = TextureWrapMode::Repeat;
		TextureWrapMode wrapModeV = TextureWrapMode::Repeat;
		TextureWrapMode wrapModeW = TextureWrapMode::Repeat;
	};

	class Texture :public ResourceBase, public Vulkan::Descriptor, public GeneralBase::NoCopyBase {
	public:
		const Vulkan::Image& getImage() const { return *mImage; }

		const vk::ImageView& getImageView() const { return mImageView; }

		const vk::Sampler& getSampler() const { return mSampler; }

		Vulkan::WriteDescriptorSet getWriteDescriptor(uint32_t _binding,
													  vk::DescriptorType _descriptorType,
													  const std::optional<OffsetSize64>& _offsetSize = std::nullopt) const override;

		virtual void apply(bool _updateMipmaps = true);

		uint32_t width(uint32_t _mipLevel = 0) const;

		uint32_t height(uint32_t _mipLevel = 0) const;

		Math::Vector2i extent(uint32_t _mipLevel = 0) const;

		uint32_t mipLevels() const { return mImage->mipLevels(); }

		uint32_t arrayLevels() const { return mImage->arrayLevels(); }

		TextureFormat format() const;

		TextureType type() const { return mType; }

		TextureWrapMode wrapModeU() const { return mSamplerInfo.wrapModeU; }
		TextureWrapMode wrapModeV() const { return mSamplerInfo.wrapModeV; }
		TextureWrapMode wrapModeW() const { return mSamplerInfo.wrapModeW; }

		static uint32_t GetMipmapLevel(uint32_t _width, uint32_t _height);

		static Math::Vector2ui GetMipmapExtent(uint32_t _width, uint32_t _height, uint32_t _mipLevel);

	protected:
		Texture(TextureType _type,
				uint32_t _width, uint32_t _height, uint32_t _depth,
				TextureFormat _format,
				uint32_t _mipLevel = 1, uint32_t _layer = 1,
				SamplerInfo _samplerInfo = {});

		std::shared_ptr<Vulkan::Image> mImage;
		vk::ImageView mImageView;
		vk::Sampler mSampler;
		SamplerInfo mSamplerInfo;
		TextureType mType;
		bool mChanged = false;

		struct CopyToDstImageInfo {
			uint32_t mipLevel;
			uint32_t layer;
			Math::Vector2ui offset;
			Math::Vector2ui extent;
		};

		std::vector<std::pair<CopyToDstImageInfo, std::shared_ptr<Vulkan::Buffer>>> mTransfers;

		static vk::Format ToVkFormat(TextureFormat _format);

		static TextureFormat FromVkFormat(vk::Format _format);

		static vk::Filter ToVkFilter(TextureFilterMode _filterMode);

		static vk::SamplerMipmapMode ToVkSampleMipMode(TextureMipSampleMode _mipSampleMode);

		static vk::SamplerAddressMode ToVkSamplerAddressMode(TextureWrapMode _wrapMode);

		static std::vector<char> GetPixels(const Vulkan::Image& _image, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, const vk::ImageSubresourceLayers& _subresource);

		static void GenMipMap(const vk::CommandBuffer& _cmd, const Vulkan::Image& _image, uint32_t _layer);
	};

	class Texture2D final :public Texture {
	public:
		Texture2D(uint32_t _width,
				  uint32_t _height,
				  TextureFormat _format,
				  uint32_t _mipLevel = 1,
				  SamplerInfo _samplerInfo = {});

		// void apply(bool _updateMipmaps = true) override;

		void setPixels(const void* _pixels, uint64_t _size, uint32_t _mipLevel = 0);

		void setPixels(const void* _pixels, uint64_t _size, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _mipLevel = 0);

		std::vector<char> getPixels(uint32_t _mipLevel = 0);

		std::vector<char> getPixels(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t _mipLevel = 0) const;
	};

	class CubeMap :public Texture {
	public:
		CubeMap(uint32_t _width,
				TextureFormat _format,
				uint32_t _mipLevel = 1,
				SamplerInfo _samplerInfo = {});

		void apply(bool _updateMipmaps = true) override;

		void setPixels(const char* _pixels, uint64_t _size, CubeMapFace _face, uint32_t _mipLevel = 0);

		void setPixels(const char* _pixels, uint64_t _size, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, CubeMapFace _face, uint32_t _mipLevel = 0);

		std::vector<char> getPixels(CubeMapFace _face, uint32_t _mipLevel = 0);

		std::vector<char> getPixels(uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, CubeMapFace _face, uint32_t _mipLevel = 0) const;

	private:
		static uint32_t GetFaceLayerIndex(CubeMapFace _face);

		void genMipmaps(const vk::CommandBuffer& _cmd) const;
	};


	// ---------------- Texture Additional Param-----------------

	struct TextureParam {
		uint32_t mipLevel = 1;
		SamplerInfo samplerInfo;
	};

	template<>
	inline auto GetAdditionalParam<Texture2D>() { return TextureParam(); }

	template<>
	inline auto GetAdditionalParam<CubeMap>() { return TextureParam(); }

}

#endif
