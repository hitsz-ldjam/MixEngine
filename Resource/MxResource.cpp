#include "MxResource.h"
#include <boost/format.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace Mix {
    namespace Resource {
        std::shared_ptr<ResourceParserBase> LoaderRegister::FindLoaderByType(const ResourceType _type) {
            for (auto& loader : mLoaders) {
                if (loader->IsSupport(_type))
                    return loader;
            }
            return nullptr;
        }

        std::shared_ptr<ResourceParserBase> LoaderRegister::FindLoaderByExt(const std::string& _ext) {
            for (auto& loader : mLoaders) {
                if (loader->IsSupport(_ext))
                    return loader;
            }
            return nullptr;
        }

        void Resources::Initialize() {
            mLoaderRegister = std::make_shared<LoaderRegister>();
        }

        std::shared_ptr<Object> Resources::Load(const std::string& _file) {
            // get lower case absolute file path
            const auto path = GetGenericPath(_file);

            if (!std::filesystem::is_regular_file(path))
                return nullptr;

            // no extension
            if (path.extension().empty()) {
                std::cerr << "Warning : " << __FUNCTION__ << ": [ " << _file << " ] has no extension." << std::endl;
                Debug::Log::Warning("%s: [ %s ] has no extension.", __FUNCTION__, _file.c_str());
                return nullptr;
            }

            // chech if file has already been loaded
            if (IsResourceAlreadyLoaded(path))
                return mResourceMap[Utils::GuidGenerator::GetGuid(path)]->ToSceneObject();

            const auto extension = path.extension().string().substr(1);
            const auto loader = mLoaderRegister->FindLoaderByExt(extension);
            if (loader == nullptr) {
                Debug::Log::Warning("%s: No loader support extenxion [ %s ].", __FUNCTION__, extension.c_str());
                return nullptr;
            }

            mResourceMap[Utils::GuidGenerator::GetGuid(path)] = loader->Load(path, extension);
            return mResourceMap[Utils::GuidGenerator::GetGuid(path)]->ToSceneObject();
        }


        std::shared_ptr<Object> Resources::Load(const std::string& _file, const ResourceType _type) {
            // get lower case absolute file path
            const auto path = GetGenericPath(_file);

            // chech if file has already been loaded
            if (IsResourceAlreadyLoaded(path))
                return mResourceMap[Utils::GuidGenerator::GetGuid(path)]->ToSceneObject();

            const auto loader = mLoaderRegister->FindLoaderByType(_type);
            if (loader == nullptr) {
                Debug::Log::Warning("%s: No loader support extenxion [ %d ].", __FUNCTION__, _type);
                return nullptr;
            }

            mResourceMap[Utils::GuidGenerator::GetGuid(path)] = loader->Load(path, _type);
            return mResourceMap[Utils::GuidGenerator::GetGuid(path)]->ToSceneObject();
        }

        void Resources::UnloadAll() {
            mResourceMap.clear();
        }

        bool Resources::IsResourceAlreadyLoaded(const std::filesystem::path& _path) const {
            return mResourceMap.count(Utils::GuidGenerator::GetGuid(_path));
        }

        std::filesystem::path Resources::GetGenericPath(const std::string& _file) {
            return std::filesystem::absolute(boost::to_lower_copy(_file)).generic_string();
        }
    }
}
