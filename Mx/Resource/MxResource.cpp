#include "MxResource.h"
#include "Model/MxGltfParser.h"

#include "../Utils/MxGuid.h"
#include "../Log/MxLog.h"
#include "../Vulkan/MxVkGraphics.h"
#include "../Utils/MxUtils.h"

namespace Mix {
    namespace Resource {
        void Resources::init() {
            mLoaderRegister = std::make_shared<ParserRegister>();

            mLoaderRegister->registerParser(std::make_shared<GltfParser>(&mResourceRefMgr));

            mResourceRefMgr.setRemoveOp([](ResourceBase* _r) {delete _r; });
        }

        Object* Resources::load(const std::string& _file) const {
            // get lower case absolute file path
            const auto path = Utils::GetGenericPath(_file);

            if (!std::filesystem::is_regular_file(path))
                return nullptr;

            // no extension
            if (path.extension().empty()) {
                Debug::Log::Warning("%s: [ %s ] has no extension.", __FUNCTION__, _file.c_str());
                return nullptr;
            }

            // chech if file has already been loaded
            const auto extension = path.extension().string().substr(1);
            const auto loader = mLoaderRegister->findLoaderByExt(extension);
            if (loader == nullptr) {
                Debug::Log::Warning("%s: No loader support extenxion [ %s ].", __FUNCTION__, extension.c_str());
                return nullptr;
            }

            if (isResourceAlreadyLoaded(path))
                return loader->parse(Utils::GuidGenerator::GetGuid(path));

            return loader->load(path, extension);
        }


        Object* Resources::load(const std::string& _file, const ResourceType _type) {
            // get lower case absolute file path
            const auto path = Utils::GetGenericPath(_file);

            // chech if file has already been loaded
            if (isResourceAlreadyLoaded(path)) {

                auto ref = mResourceRefMgr.getReference(Utils::GuidGenerator::GetGuid(path));

            }

            const auto loader = mLoaderRegister->findLoaderByType(_type);
            if (loader == nullptr) {
                Debug::Log::Warning("%s: No loader support extenxion [ %d ].", __FUNCTION__, _type);
                return nullptr;
            }

            if (isResourceAlreadyLoaded(path))
                return loader->parse(Utils::GuidGenerator::GetGuid(path));

            return loader->load(path, _type);
        }

        void Resources::unloadAll() {
            mResourceRefMgr.clear();
        }

        bool Resources::isResourceAlreadyLoaded(const std::filesystem::path& _path) const {
            return mResourceRefMgr.hasKey(Utils::GuidGenerator::GetGuid(_path));
        }
    }
}
