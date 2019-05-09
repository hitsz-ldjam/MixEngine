#pragma once
#ifndef MX_RESOURCEBASE_H_
#define MX_RESOURCEBASE_H_

#include <utility>
#include "../Utils/MxUtils.h"
#include "../Utils/MxReferenceMgr.h"

namespace Mix {
    namespace Resource {
        enum class ResourceType {
            UNKNOWN,
            GLTF_BIN,
            GLTF_ASCII,
            PNG,
            JPG
        };

        class ResourceBase;
        typedef Utils::ReferenceMgr<Guid, std::shared_ptr<ResourceBase>> ResourceRefMgr;
        typedef Utils::Reference<ResourceRefMgr> ResourceRef;

        class ResourceBase {
        public:
            ResourceBase(const Guid _guid, std::string _fileName, std::filesystem::path _filePath) :
                mGuid(_guid), mFileName(std::move(_fileName)), mFilePath(std::move(_filePath)) {
            }

            ResourceBase(const ResourceBase& _other) = delete;

            ResourceBase(ResourceBase&& _other) = default;

            ResourceBase& operator=(const ResourceBase& _other) = delete;

            ResourceBase& operator=(ResourceBase&& _other) = default;

            virtual ~ResourceBase() = default;

            Guid GetGuid() const { return mGuid; }

            const std::string& FileName()const {
                return mFileName;
            }

        protected:
            Guid mGuid = 0;
            std::string mFileName;
            std::filesystem::path mFilePath;

        };

    }
}

#endif
