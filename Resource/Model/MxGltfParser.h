#pragma once
#ifndef MX_RESOURCE_LOADER_H_
#define MX_RESOURCE_LOADER_H_

#include "MxResModel.h"
#include "MxModelParserBase.hpp"
#include "../../MxVk/MxVkGraphics.h"

namespace Mix {
    namespace Resource {
        class GltfParser final : public ModelParseBase {
        public:
            explicit GltfParser(ResourceRefMgr* _mgr) :ModelParseBase(_mgr) {
                mSupportedTypes.insert(ResourceType::GLTF_BIN);
                mSupportedTypes.insert(ResourceType::GLTF_ASCII);

                mSupportedExts.insert("gltf");
                mSupportedExts.insert("glb");

                auto vulkan = Object::FindObjectOfType<Graphics::Vulkan>();
                mFence = vulkan->getCore()->getSyncObjMgr().createFence();
                vulkan->getLogicalDevice().resetFences(mFence.get());
            }

            std::shared_ptr<Object> load(const std::filesystem::path& _path, const ResourceType _type) override;

            std::shared_ptr<Object> load(const std::filesystem::path& _path, const std::string& _ext) override;

            std::shared_ptr<Object> parse(const Guid& _key) override;
        private:
            Utils::GltfLoader mLoader;
            Graphics::Fence mFence;

            ResModel* parseModelData(const std::filesystem::path&        _path,
                                     const Utils::GltfLoader::ModelData& _modelData);
        };
    }
}

#endif // !MX_RESOURCE_LOADER_H_
