#pragma once
#ifndef MX_MATERIAL_H_
#define MX_MATERIAL_H_

#include "../Vulkan/Descriptor/MxVkDescriptor.h"
#include <unordered_map>
#include <string>

namespace Mix {
	namespace Graphics {
		class Pipeline;
	}

	namespace Resource {
		class Texture;
	};

	class Material {
	public:
		/*void setTexture(const std::string& _name, std::shared_ptr<Resource::Texture> _texture);

		std::shared_ptr<Resource::Texture> getTexture(const std::string& _name) const;

		const vk::DescriptorSet& getDescriptorSet() const;*/

	private:
		std::unordered_map<std::string, std::shared_ptr<Resource::Texture>> mTextures;
		std::shared_ptr<Graphics::Pipeline> mPipeline;
	};
}

#endif
