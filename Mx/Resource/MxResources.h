#pragma once
#ifndef MX_RESOURCES_H_
#define MX_RESOURCES_H_

#include "../Engine/MxModuleBase.h"
#include "../Definitions/MxDefinitions.h"
#include <mutex>
#include "../Utils/MxGuid.h"
#include "MxResourceHandle.h"

namespace Mix {
    struct LoadOptions {
        LoadOptions() = default;

        virtual ~LoadOptions() = default;
    };


    class Resources :public ModuleBase {
    public:

        HResource load(const std::filesystem::path& _path, std::shared_ptr<LoadOptions> _options = nullptr);

        template<typename _Ty>
        ResourceHandle<_Ty> load(const std::filesystem::path& _path, std::shared_ptr<LoadOptions> _options = nullptr);

        HResource loadAsync(const std::filesystem::path& _path, std::shared_ptr<LoadOptions> _options = nullptr);

        template<typename _Ty>
        ResourceHandle<_Ty> loadAsync(const std::filesystem::path& _path, std::shared_ptr<LoadOptions> _options = nullptr);

    private:

        struct LoadedResource {
            WeakResourceHandle<Resource> resource;
        };

        struct ResourceInLoading {
            std::shared_ptr<Resource> resource;
            bool loadStarted = false;
        };


        //std::unordered_map<UUID, >

    };
}

#endif
