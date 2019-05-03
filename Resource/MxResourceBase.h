#pragma once
#ifndef MX_RESOURCEBASE_H_
#define MX_RESOURCEBASE_H_

#include <utility>
#include "../Utils/MxUtils.h"

namespace Mix {
    namespace Resource {
        enum class ResourceType {
            UNKNOWN,
            GLTF_BIN,
            GLTF_ASCII,
            PNG,
            JPG
        };


        class ResourceBase {
        public:
            ResourceBase(const Guid _guid, std::string _fileName, std::filesystem::path& _filePath) :
                mGuid(_guid), mFileName(std::move(_fileName)), mFilePath(_filePath) {
            }

            ResourceBase(const ResourceBase& _other) = delete;

            ResourceBase(ResourceBase&& _other) = default;

            ResourceBase& operator=(const ResourceBase& _other) = delete;

            ResourceBase& operator=(ResourceBase&& _other) = default;

            virtual ~ResourceBase() = default;

            virtual std::shared_ptr<Object> ToSceneObject() = 0;

            Guid GetGuid() const { return mGuid; }

            const std::string& FileName()const {
                return mFileName;
            }

        private:
            Guid mGuid = 0;
            std::string mFileName;
            std::filesystem::path mFilePath;

        };
    }
}

#endif
