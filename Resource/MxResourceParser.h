#pragma once
#ifndef MX_RESOURCE_LOADER_H_
#define MX_RESOURCE_LOADER_H_

#include <set>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include "../Mx/MxGameObject.h"
#include "MxResourceBase.h"
#include "../MxVk/MxVkGraphics.h"
#include "../Utils/MxReferenceMgr.h"
#include "../Engine/MxLog.h"
#include "MxResModel.h"

namespace Mix {
    namespace Resource {
        class ResourceParserBase {
        public:
            explicit ResourceParserBase(ResourceRefMgr* _mgr) :mRefMgr(_mgr) {}

            ResourceParserBase(const ResourceParserBase& _other) = default;

            ResourceParserBase(ResourceParserBase&& _other) noexcept = default;

            ResourceParserBase& operator=(const ResourceParserBase& _other) = default;

            ResourceParserBase& operator=(ResourceParserBase&& _other) noexcept = default;

            virtual std::shared_ptr<Object> Load(const std::filesystem::path& _path, const ResourceType _type) = 0;

            virtual std::shared_ptr<Object> Load(const std::filesystem::path& _path, const std::string& _ext) = 0;

            virtual std::shared_ptr<Object> Parse(const Guid& _key) = 0;

            virtual ~ResourceParserBase() = default;

            bool IsSupport(const ResourceType _type) const {
                return mSupportedTypes.count(_type);
            }

            bool IsSupport(const std::string& _ext) const {
                return mSupportedExts.count(_ext);
            }

            const std::set<ResourceType>& AllSupportedTypes() const {
                return mSupportedTypes;
            }

            const std::unordered_set<std::string>& AllSupportedExts()const {
                return mSupportedExts;
            }

        protected:
            ResourceRefMgr* mRefMgr;
            std::set<ResourceType> mSupportedTypes;
            std::unordered_set<std::string> mSupportedExts;

        };


        class ModelParseBase :public ResourceParserBase {
        protected:
            explicit ModelParseBase(ResourceRefMgr* _mgr) :ResourceParserBase(_mgr) {}
        };


        class GltfParser final : public ModelParseBase {
        public:
            explicit GltfParser(ResourceRefMgr* _mgr) :ModelParseBase(_mgr) {
                mSupportedTypes.insert(ResourceType::GLTF_BIN);
                mSupportedTypes.insert(ResourceType::GLTF_ASCII);

                mSupportedExts.insert("gltf");
                mSupportedExts.insert("glb");
            }

            std::shared_ptr<Object> Load(const std::filesystem::path& _path, const ResourceType _type) override;

            std::shared_ptr<Object> Load(const std::filesystem::path& _path, const std::string& _ext) override;

            std::shared_ptr<Object> Parse(const Guid& _key) override;
        private:
            Utils::GltfLoader mLoader;
            Graphics::Fence mFence;

            std::shared_ptr<ResModel> ParseModelData(const std::filesystem::path&        _path,
                                                   const Utils::GltfLoader::ModelData& _modelData);
        };
    }
}

#endif // !MX_RESOURCE_LOADER_H_
