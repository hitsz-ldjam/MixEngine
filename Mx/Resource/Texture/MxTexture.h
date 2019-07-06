#pragma once
#pragma once
#ifndef MX_TEXTURE_H_
#define MX_TEXTURE_H_

#include "../../Vulkan/Image/MxVkImage.h"
#include "../MxResourceBase.h"
#include <memory>

namespace Mix {
	namespace Resource {
		class Texture :public ResourceBase {
			friend class TextureParserBase;
		public:
			struct DataType {
				std::shared_ptr<Graphics::Image> image;
				vk::UniqueImageView imageView;
				vk::UniqueSampler sampler;
			};

			const Graphics::Image& getImage() const { return *mData->image; }

			const vk::ImageView& getImageView() const { return *mData->imageView; }

			const vk::Sampler& getSampler() const { return *mData->sampler; }

		protected:
			Texture() :mData(std::make_shared<DataType>()) {}

			std::shared_ptr<DataType> mData;
		};

		class Texture2D final :public Texture {
		public:
			Texture2D(uint32_t _width,
					  uint32_t _height,
					  vk::Format _format,
					  const vk::DeviceSize& _size,
					  const char* _data = nullptr,
					  std::optional<vk::SamplerCreateInfo> _samplerInfo = std::nullopt);

			uint32_t width() const {
				return mData->image->extent().width;
			}

			uint32_t height() const {
				return mData->image->extent().height;
			}

			vk::Format format() const {
				return mData->image->format();
			}

			void uploadData(const char* const _data, const vk::DeviceSize& _size);
		};

		class CubeMap :public Texture {
		public:
			CubeMap(uint32_t _width,
					uint32_t _height,
					vk::Format _format,
					const char* _data = nullptr,
					const vk::DeviceSize& _size = 0,
					std::optional<vk::SamplerCreateInfo> _samplerInfo = std::nullopt){};

			uint32_t width() const {
				return mData->image->extent().width;
			}

			uint32_t height() const {
				return mData->image->extent().height;
			}

			vk::Format format() const {
				return mData->image->format();
			}

			void uploadData(const char* const _data, const vk::DeviceSize& _size){};
		};
	}
}

#endif
